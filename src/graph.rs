pub mod egui_multiwin_dynamic {
    egui_multiwin::tracked_window!(crate::AppCommon, crate::CustomEvent, crate::MyWindows);
    egui_multiwin::multi_window!(crate::AppCommon, crate::CustomEvent, crate::MyWindows);
}

#[enum_dispatch(TrackedWindow)]
pub enum MyWindows {
    Root(RootWindow),
}

use egui_multiwin::arboard;
use egui_multiwin::egui_glow::EguiGlow;
use egui_multiwin::enum_dispatch::enum_dispatch;
use egui_multiwin_dynamic::multi_window::NewWindowRequest;
use egui_multiwin_dynamic::tracked_window::RedrawResponse;
use egui_multiwin_dynamic::tracked_window::TrackedWindow;
use graphviz_rust::printer::PrinterContext;
use std::sync::Arc;

use graphviz_rust::dot_generator::*;
use graphviz_rust::dot_structures::Graph;
use graphviz_rust::dot_structures::*;

pub struct AppCommon {
    clicks: u32,
}

pub struct GraphIterator {
    gg: GraphGenerator,
    u: u32,
}

impl Iterator for GraphIterator {
    type Item = Graph;

    fn next(&mut self) -> Option<Self::Item> {
        let mut g = graph!(di id!("generated"));
        for i in 0..self.gg.num_blocks {
            let s = format!("{}", i);
            let id = graphviz_rust::dot_structures::Id::Plain(s);
            let id = graphviz_rust::dot_structures::NodeId(id, None);
            let e = graphviz_rust::dot_structures::Node {
                id,
                attributes: vec![],
            };
            g.add_stmt(graphviz_rust::dot_structures::Stmt::Node(e));
        }
        Some(g)
    }
}

#[derive(Copy, Clone)]
pub struct GraphGenerator {
    num_blocks: u32,
}

impl GraphGenerator {
    pub fn new(num_blocks: u32) -> Self {
        Self { num_blocks }
    }

    pub fn create_iter(&self) -> GraphIterator {
        GraphIterator { gg: *self, u: 42 }
    }
}

#[derive(Debug)]
pub struct CustomEvent {
    window: Option<egui_multiwin::winit::window::WindowId>,
    message: u32,
}

impl CustomEvent {
    fn window_id(&self) -> Option<egui_multiwin::winit::window::WindowId> {
        self.window
    }
}

pub struct RootWindow {
    done: bool,
    generated: bool,
    redraw: bool,
    image_is_recent: bool,
    gg: GraphGenerator,
    gi: GraphIterator,
    graph: Graph,
    png: Vec<u8>,
    texture: Option<egui_multiwin::egui::TextureHandle>,
}

impl RootWindow {
    pub fn request() -> NewWindowRequest {
        let gg = GraphGenerator::new(3);
        let mut gi = gg.create_iter();
        let graph = gi.next().unwrap();

        NewWindowRequest {
            window_state: MyWindows::Root(RootWindow {
                done: false,
                generated: true,
                image_is_recent: false,
                redraw: false,
                gg,
                gi,
                graph,
                png: vec![],
                texture: None,
            }),
            builder: egui_multiwin::winit::window::WindowBuilder::new()
                .with_resizable(false)
                .with_inner_size(egui_multiwin::winit::dpi::LogicalSize {
                    width: 400.0,
                    height: 200.0,
                })
                .with_title("A window"),
            options: egui_multiwin::tracked_window::TrackedWindowOptions {
                vsync: false,
                shader: None,
            },
            id: egui_multiwin::multi_window::new_id(),
        }
    }
}

impl TrackedWindow for RootWindow {
    fn is_root(&self) -> bool {
        true
    }

    fn redraw(
        &mut self,
        c: &mut AppCommon,
        egui: &mut EguiGlow,
        _window: &egui_multiwin::winit::window::Window,
        _clipboard: &mut arboard::Clipboard,
    ) -> RedrawResponse {
        let quit = false;

        if !self.generated {
            if let Some(g) = self.gi.next() {
                self.graph = g;
                self.image_is_recent = false;
            } else {
                self.done = true;
            }
            self.generated = true;
        }

        if !self.image_is_recent {
            let graph_png = graphviz_rust::exec(
                self.graph.clone(),
                &mut PrinterContext::default(),
                vec![graphviz_rust::cmd::Format::Png.into()],
            )
            .unwrap();
            self.png = graph_png;
            self.redraw = true;
            self.image_is_recent = true;
        }

        if self.redraw {
            println!("Image buf size is {}", self.png.len());
            std::fs::write("./generated.png", &self.png).unwrap();
            let options =
                zune_png::zune_core::options::DecoderOptions::default().png_set_strip_to_8bit(true);
            let mut decoder = zune_png::PngDecoder::new_with_options(&self.png, options);
            decoder.decode_headers().unwrap();
            let (w, h) = decoder.get_dimensions().unwrap();
            println!("Dimensions are {}x{}", w, h);

            let mut pixels: Vec<u8> = vec![0; w * h * 4];

            println!("Decode into buffer: {:?}", decoder.decode_into(pixels.as_mut()));

            let pixels: Vec<egui_multiwin::egui::Color32> = pixels
                .chunks_exact(4)
                .map(|raw| egui_multiwin::egui::Color32::from_rgb(raw[0], raw[1], raw[2]))
                .collect();

            println!("There are {} pixels", pixels.len());

            let image = egui_multiwin::egui::ColorImage {
                size: [w, h],
                pixels,
            };

            if self.texture.is_none() {
                self.texture = Some(egui.egui_ctx.load_texture(
                    "graph",
                    image,
                    egui_multiwin::egui::TextureOptions::NEAREST,
                ));
            } else if let Some(t) = &mut self.texture {
                if t.size()[0] != image.width() || t.size()[1] != image.height() {
                    self.texture = Some(egui.egui_ctx.load_texture(
                        "graph",
                        image,
                        egui_multiwin::egui::TextureOptions::NEAREST,
                    ));
                } else {
                    t.set_partial([0, 0], image, egui_multiwin::egui::TextureOptions::NEAREST);
                }
            }
            self.redraw = false;
        }

        egui_multiwin::egui::CentralPanel::default().show(&egui.egui_ctx, |ui| {
            ui.heading(format!("number {}", c.clicks));
            if let Some(t) = &self.texture {
                ui.add(egui_multiwin::egui::Image::from_texture(
                    egui_multiwin::egui::load::SizedTexture {
                        id: t.id(),
                        size: egui_multiwin::egui::Vec2 {
                            x: t.size()[0] as f32,
                            y: t.size()[1] as f32,
                        },
                    },
                ));
            }
        });
        RedrawResponse {
            quit,
            new_windows: Vec::new(),
        }
    }
}

impl AppCommon {
    fn process_event(&mut self, _event: CustomEvent) -> Vec<NewWindowRequest> {
        vec![]
    }
}

fn main() {
    egui_multiwin_dynamic::multi_window::MultiWindow::start(|multi_window, event_loop, _proxy| {
        let root_window = RootWindow::request();

        let mut ac = AppCommon { clicks: 0 };

        if let Err(e) = multi_window.add(root_window, &mut ac, event_loop) {
            println!("Failed to create main window {:?}", e);
        }
        ac
    });
}
