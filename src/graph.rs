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
    links: Vec<(Option<u32>, Option<u32>)>,
    done: bool,
}

impl GraphIterator {
    pub fn advance(&mut self) {
        if self.done {
            return;
        }
        for (elem, elem2) in self.links.iter_mut() {
            *elem = match *elem {
                None => Some(0),
                Some(a) => {
                    if (a + 1) < self.gg.num_blocks {
                        Some(a + 1)
                    } else {
                        None
                    }
                }
            };
            if elem.is_none() {
                *elem2 = match *elem2 {
                    None => Some(0),
                    Some(a) => {
                        if (a + 1) < self.gg.num_blocks {
                            Some(a + 1)
                        } else {
                            None
                        }
                    }
                };
                if elem2.is_some() {
                    break;
                }
            } else {
                break;
            }
        }
    }

    pub fn check(&self) -> bool {
        let mut valid = true;
        for (a, b) in self.links.iter() {
            if a == b && a.is_some() {
                valid = false;
            }
            if a.is_none() && b.is_some() {
                valid = false;
            }
            if let Some(a) = a {
                if let Some(b) = b {
                    if a > b {
                        valid = false;
                    }
                }
            }
        }

        let (a, b) = self.links[self.gg.num_blocks as usize - 1];
        if a.is_some() || b.is_some() {
            valid = false;
        }

        let mut inputs = vec![0; self.gg.num_blocks as usize];
        for (index, (elem, elem2)) in self.links.iter().enumerate() {
            if let Some(a) = *elem {
                if index != a as usize {
                    inputs[a as usize] += 1;
                }
            }
            if let Some(a) = *elem2 {
                if index != a as usize {
                    inputs[a as usize] += 1;
                }
            }
        }
        for i in inputs.iter().skip(1) {
            if *i == 0 {
                valid = false;
            }
        }

        valid
    }

    pub fn checked_advance(&mut self) {
        loop {
            self.advance();
            if self.done() {
                self.done = true;
                break;
            }
            if self.check() {
                break;
            }
        }
    }

    pub fn done(&self) -> bool {
        let mut done = true;
        for (a, b) in self.links.iter() {
            if a.is_some() || b.is_some() {
                done = false;
                break;
            }
        }
        done
    }
}

impl Iterator for GraphIterator {
    type Item = Graph;

    fn next(&mut self) -> Option<Self::Item> {
        self.checked_advance();
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
        for (i, (la, lb)) in self.links.iter().enumerate() {
            if let Some(link) = la {
                let s = format!("{}", i);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id1 = graphviz_rust::dot_structures::NodeId(id, None);

                let s = format!("{}", link);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id2 = graphviz_rust::dot_structures::NodeId(id, None);

                let va = graphviz_rust::dot_structures::Vertex::N(id1);
                let vb = graphviz_rust::dot_structures::Vertex::N(id2);
                let t = graphviz_rust::dot_structures::EdgeTy::Pair(va, vb);
                let e = graphviz_rust::dot_structures::Edge {
                    ty: t,
                    attributes: Vec::new(),
                };
                g.add_stmt(graphviz_rust::dot_structures::Stmt::Edge(e));
            }
            if let Some(link) = lb {
                let s = format!("{}", i);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id1 = graphviz_rust::dot_structures::NodeId(id, None);

                let s = format!("{}", link);
                let id = graphviz_rust::dot_structures::Id::Plain(s);
                let id2 = graphviz_rust::dot_structures::NodeId(id, None);

                let va = graphviz_rust::dot_structures::Vertex::N(id1);
                let vb = graphviz_rust::dot_structures::Vertex::N(id2);
                let t = graphviz_rust::dot_structures::EdgeTy::Pair(va, vb);
                let e = graphviz_rust::dot_structures::Edge {
                    ty: t,
                    attributes: Vec::new(),
                };
                g.add_stmt(graphviz_rust::dot_structures::Stmt::Edge(e));
            }
        }
        if self.done() {
            None
        }
        else {
            Some(g)
        }
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
        GraphIterator {
            gg: *self,
            links: vec![(None, None); self.num_blocks as usize],
            done: false,
        }
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
            if ui
                .button(format!("Move to {} nodes", self.gg.num_blocks + 1))
                .clicked()
            {
                self.gg = GraphGenerator::new(self.gg.num_blocks + 1);
                self.gi = self.gg.create_iter();
                self.generated = false;
            }
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
