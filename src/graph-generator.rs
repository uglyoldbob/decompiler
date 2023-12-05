pub mod egui_multiwin_dynamic {
    egui_multiwin::tracked_window!(crate::AppCommon, crate::CustomEvent, crate::MyWindows);
    egui_multiwin::multi_window!(crate::AppCommon, crate::CustomEvent, crate::MyWindows);
}

mod generator;

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
use graphviz_rust::dot_structures::Graph;
use graphviz_rust::printer::PrinterContext;
use std::sync::Arc;

pub struct AppCommon {
    clicks: u32,
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
    gg: generator::GraphGenerator,
    gi: generator::GraphIterator,
    graph: Graph,
    png: Vec<u8>,
    texture: Option<egui_multiwin::egui::TextureHandle>,
}

impl RootWindow {
    pub fn request() -> NewWindowRequest {
        let gg = generator::GraphGenerator::new(3);
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
                .with_resizable(true)
                .with_inner_size(egui_multiwin::winit::dpi::LogicalSize {
                    width: 640.0,
                    height: 480.0,
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
            let options =
                zune_png::zune_core::options::DecoderOptions::default().png_set_strip_to_8bit(true);
            let mut decoder = zune_png::PngDecoder::new_with_options(&self.png, options);
            decoder.decode_headers().unwrap();
            let (w, h) = decoder.get_dimensions().unwrap();

            let mut pixels: Vec<u8> = vec![0; w * h * 4];

            decoder.decode_into(pixels.as_mut());

            let pixels: Vec<egui_multiwin::egui::Color32> = pixels
                .chunks_exact(4)
                .map(|raw| egui_multiwin::egui::Color32::from_rgb(raw[0], raw[1], raw[2]))
                .collect();

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
            if ui.button("Next graph").clicked() {
                self.generated = false;
            }
            if self.gg.num_blocks() > 1 {
                if ui
                    .button(format!("Move to {} nodes", self.gg.num_blocks() - 1))
                    .clicked()
                {
                    self.gg = generator::GraphGenerator::new(self.gg.num_blocks() - 1);
                    self.gi = self.gg.create_iter();
                    self.generated = false;
                }
            }
            if ui
                .button(format!("Move to {} nodes", self.gg.num_blocks() + 1))
                .clicked()
            {
                self.gg = generator::GraphGenerator::new(self.gg.num_blocks() + 1);
                self.gi = self.gg.create_iter();
                self.generated = false;
            }
            if let Some(t) = &self.texture {
                egui_multiwin::egui::ScrollArea::both()
                    .auto_shrink([false; 2])
                    .show(ui, |ui| {
                        ui.add(egui_multiwin::egui::Image::from_texture(
                            egui_multiwin::egui::load::SizedTexture {
                                id: t.id(),
                                size: egui_multiwin::egui::Vec2 {
                                    x: t.size()[0] as f32,
                                    y: t.size()[1] as f32,
                                },
                            },
                        ));
                    });
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
