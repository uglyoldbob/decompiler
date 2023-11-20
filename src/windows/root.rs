//! The root window for the decompiler. This is the first window that a user will see and the primary window for interacting with the program.

use std::{collections::HashSet, path::PathBuf};

use crate::egui_multiwin_dynamic::{
    multi_window::NewWindowRequest,
    tracked_window::{RedrawResponse, TrackedWindow},
};
use egui_multiwin::egui;

use egui_multiwin::egui_glow::EguiGlow;
use object::{Object, ObjectSection};

use crate::MyApp;

/// The main window for the decompiler.
pub struct RootWindow {
    /// The list of files dropped onto the window.
    dropped_files: HashSet<PathBuf>,
    /// The index of the currently selected file in the sidebar
    selected_object: Option<usize>,
    /// The index of the object selected on the tab select
    current_object_index: Option<usize>,
    /// The set of files that are currently open for analysis out of all files in the current project.
    open_files: HashSet<usize>,
}

impl RootWindow {
    /// Construct a request to build a new window.
    pub fn request() -> NewWindowRequest {
        NewWindowRequest {
            window_state: super::MyWindows::Root(RootWindow {
                dropped_files: HashSet::new(),
                selected_object: None,
                open_files: HashSet::new(),
                current_object_index: None,
            }),
            builder: egui_multiwin::winit::window::WindowBuilder::new()
                .with_resizable(true)
                .with_inner_size(egui_multiwin::winit::dpi::LogicalSize {
                    width: 800.0,
                    height: 600.0,
                })
                .with_title("egui-multiwin root window"),
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

    fn set_root(&mut self, _root: bool) {}

    fn redraw(
        &mut self,
        c: &mut MyApp,
        egui: &mut EguiGlow,
        _window: &egui_multiwin::winit::window::Window,
        _clipboard: &mut egui_multiwin::arboard::Clipboard,
    ) -> RedrawResponse {
        let quit = false;

        let windows_to_create = vec![];

        egui::TopBottomPanel::top("my_panel").show(&egui.egui_ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    let button = egui::Button::new("New Project");

                    if ui.add_enabled(true, button).clicked() {
                        let file = rfd::FileDialog::new()
                            .set_directory("/")
                            .set_title("New Project")
                            .pick_folder();

                        if let Some(f) = file {
                            let p = crate::Project::new(f);
                            p.setup();

                            c.project = Some(p);
                        }

                        ui.close_menu();
                    }

                    let button = egui::Button::new("Open Project");

                    if ui.add_enabled(true, button).clicked() {
                        let file = rfd::FileDialog::new()
                            .set_directory("/")
                            .set_title("Open Project")
                            .pick_folder();

                        if let Some(f) = file {
                            let p = crate::Project::open_project(f);
                            if p.is_some() {
                                c.project = p;
                            }
                        }

                        ui.close_menu();
                    }

                    let button = egui::Button::new("Close Project");

                    if ui.add_enabled(c.project.is_some(), button).clicked() {
                        c.project = None;
                        self.selected_object = None;
                        self.open_files = HashSet::new();
                        ui.close_menu();
                    }
                });
            });
        });

        if let Some(f) = &mut c.project {
            let sp = egui::SidePanel::left("my_left_panel").resizable(true);

            let mut open_file = None;

            sp.show(&egui.egui_ctx, |ui| {
                egui::ScrollArea::both().show(ui, |ui| {
                    ui.label("Input Files");

                    ui.group(|ui| {
                        for (k, n) in f.infiles.iter() {
                            let r = ui.selectable_label(
                                self.selected_object.eq(&Some(*k)),
                                n.to_string(),
                            );

                            if r.clicked() {
                                self.selected_object = Some(*k);
                            }

                            if r.double_clicked() {
                                open_file = Some(*k);

                                self.open_files.insert(*k);
                            }
                        }
                    });
                });

                if let Some(i) = self.current_object_index {
                    if let Some(file) = f.open_files.get(&i) {
                        ui.label("Elements");

                        ui.group(|ui| {
                            egui::ScrollArea::both().show(ui, |ui| {
                                let obj = file.borrow_obj();
                                for i in object::Object::sections(obj) {
                                    ui.label(format!("Section {:?}", i));
                                }
                            });
                        });
                    }
                }
            });

            if let Some(file) = open_file {
                f.open_file(file);
                f.trigger_parse_file(file);
            }
        }

        if let Some(f) = &mut c.project {
            egui::TopBottomPanel::top("file_selection_bar").show(&egui.egui_ctx, |ui| {
                let mut remove_elements = Vec::new();

                ui.horizontal(|ui| {
                    for i in &self.open_files {
                        ui.group(|ui| {
                            if ui
                                .selectable_label(
                                    self.current_object_index == Some(*i),
                                    &f.infiles[i][..],
                                )
                                .clicked()
                            {
                                self.current_object_index = Some(*i);
                            }

                            if ui.button("✖").clicked() {
                                if self.current_object_index == Some(*i) {
                                    self.current_object_index = None;
                                }

                                remove_elements.push(*i);
                            }
                        });
                    }
                });

                for i in remove_elements {
                    f.close_file(i);

                    self.open_files.remove(&i);
                }
            });
        }

        egui::CentralPanel::default().show(&egui.egui_ctx, |ui| {
            egui::ScrollArea::vertical().show(ui, |ui| {
                if let Some(f) = &c.project {
                    if let Some(i) = self.current_object_index {
                        if let Some(file) = f.open_files.get(&i) {
                            ui.label(file.borrow_name().to_string());
                            let obj = file.borrow_obj();
                            ui.label(format!("Entry point is {:X}", obj.entry()));
                            for i in object::Object::sections(obj) {
                                ui.label(format!("Section {:?}", i));
                                let start = i.address();
                                let end = i.address() + i.size();
                                ui.label(format!("Searching {}..{}", start, end));
                                if (start..end).contains(&obj.entry()) {
                                    ui.label("FOUND ENTRY SECTION");
                                }
                            }
                        }
                    }
                }
            });
        });

        c.preview_files_being_dropped(&egui.egui_ctx);

        // Collect dropped files:
        if let Some(prj) = &mut c.project {
            for f in &egui.egui_ctx.input(|i| i.raw.dropped_files.clone()) {
                if let Some(p) = &f.path {
                    prj.copy_input_file(p);
                    self.dropped_files.insert(p.clone());
                }
            }
        }

        RedrawResponse {
            quit,
            new_windows: windows_to_create,
        }
    }
}
