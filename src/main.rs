#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")] // hide console window on Windows in release

use std::collections::HashMap;
use std::collections::HashSet;
use std::io::Read;
use std::ops::Index;
use std::path::PathBuf;

use eframe;
use eframe::egui;
use ouroboros::self_referencing;

fn main() {
    let options = eframe::NativeOptions::default();
    eframe::run_native(
        "Decompiler",
        options,
        Box::new(|_cc| Box::new(MyApp::default())),
    );
}

pub struct AutoHashMap<T> {
    d: HashMap<usize, T>,
    next: usize,
}

impl<T> AutoHashMap<T> {
    pub fn new() -> Self {
        Self {
            d: HashMap::new(),
            next: 0,
        }
    }

    pub fn iter(&self) -> std::collections::hash_map::Iter<'_, usize, T> {
        self.d.iter()
    }

    pub fn len(&self) -> usize {
        self.d.len()
    }

    pub fn get(&self, k: &usize) -> Option<&T> {
        self.d.get(k)
    }

    pub fn insert(&mut self, v: T) {
        self.d.insert(self.next, v);
        self.next += 1;
    }
}

impl<T> Index<&usize> for AutoHashMap<T> {
    type Output = T;
    fn index(&self, k: &usize) -> &Self::Output {
        &self.d[k]
    }
}

#[self_referencing]
pub struct ProjectInputFile {
    pub name: String,
    f: std::fs::File,
    data: Vec<u8>,
    #[borrows(data)]
    #[covariant]
    pub obj: object::File<'this>,
}

pub struct Project {
    pub path: PathBuf,
    pub inputs: PathBuf,
    pub infiles: AutoHashMap<String>,
    pub open_files: HashMap<usize, ProjectInputFile>,
}

impl Project {
    pub fn new(p: PathBuf) -> Self {
        let mut inp = p.clone();
        inp.push("input");
        Self {
            path: p,
            inputs: inp,
            infiles: AutoHashMap::new(),
            open_files: HashMap::new(),
        }
    }

    pub fn open_project(p: PathBuf) -> Option<Self> {
        let mut prj = Project::new(p);
        let entries = std::fs::read_dir(&prj.inputs);
        if let Ok(entries) = entries {
            for e in entries.into_iter() {
                if let Ok(e) = e {
                    let path = e.path();
                    let meta = std::fs::metadata(&path).unwrap();
                    if meta.is_file() {
                        prj.infiles
                            .insert(e.file_name().to_str().unwrap().to_string());
                    }
                }
            }
            Some(prj)
        } else {
            None
        }
    }

    pub fn setup(&self) {
        std::fs::create_dir(&self.inputs);
    }

    pub fn copy_input_file(&mut self, p: &PathBuf) {
        let name = p.file_stem();
        if let Some(n) = name {
            let s = n.to_str().unwrap();
            let ext = if let Some(e) = p.extension() {
                e.to_str().unwrap()
            } else {
                ""
            };
            let s = format!("{}_{}.{}", s, self.infiles.len(), ext);
            let mut p2 = self.inputs.clone();
            p2.push(&s);
            std::fs::copy(p, p2);
            self.infiles.insert(s);
        }
    }

    pub fn open_file(&mut self, index: usize) -> Result<(), ()> {
        if !self.open_files.contains_key(&index) {
            if let Some(name) = self.infiles.get(&index) {
                let mut fp = self.inputs.clone();
                fp.push(name);
                let f = std::fs::File::open(fp);
                if let Ok(mut file) = f {
                    let mut v = Vec::new();
                    file.read_to_end(&mut v);
                    let mut nf = ProjectInputFileBuilder {
                        data: v,
                        obj_builder: |a| object::File::parse(&a[..]).unwrap(),
                        name: name.clone(),
                        f: file,
                    }
                    .build();
                    self.open_files.insert(index, nf);
                    return Ok(());
                } else {
                    return Err(());
                }
            } else {
                return Err(());
            }
        } else {
            return Err(());
        }
    }

    pub fn close_file(&mut self, index: usize) {
        if !self.open_files.contains_key(&index) {
            if let Some(name) = self.infiles.get(&index) {
                self.open_files.remove(&index);
            }
        }
    }
}

struct MyApp {
    dropped_files: HashSet<PathBuf>,

    show_input_files: bool,

    project: Option<Project>,

    selected_object: Option<usize>,

    current_object_index: Option<usize>,

    open_files: HashSet<usize>,

    show_invalid_project: bool,
}

impl Default for MyApp {
    fn default() -> Self {
        Self {
            dropped_files: HashSet::new(),

            show_input_files: false,

            project: None,

            selected_object: None,

            open_files: HashSet::new(),

            current_object_index: None,

            show_invalid_project: false,
        }
    }
}

impl MyApp {
    fn preview_files_being_dropped(&mut self, ctx: &egui::Context) {
        use egui::*;

        use std::fmt::Write as _;

        if !ctx.input().raw.hovered_files.is_empty() {
            let mut text = "Dropping files:\n".to_owned();

            for file in &ctx.input().raw.hovered_files {
                if let Some(path) = &file.path {
                    write!(text, "\n{}", path.display()).ok();
                } else if !file.mime.is_empty() {
                    write!(text, "\n{}", file.mime).ok();
                } else {
                    text += "\n???";
                }
            }

            let painter =
                ctx.layer_painter(LayerId::new(Order::Foreground, Id::new("file_drop_target")));

            let screen_rect = ctx.input().screen_rect();

            painter.rect_filled(screen_rect, 0.0, Color32::from_black_alpha(192));

            painter.text(
                screen_rect.center(),
                Align2::CENTER_CENTER,
                text,
                TextStyle::Heading.resolve(&ctx.style()),
                Color32::WHITE,
            );
        }
    }
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _frame: &mut eframe::Frame) {
        if self.show_input_files {
            egui::Window::new("Input Files 3")
                .collapsible(false)
                .show(ctx, |ui| {
                    ui.label("Hello World 3!");

                    if !self.dropped_files.is_empty() {
                        ui.group(|ui| {
                            ui.label("Dropped files:");

                            for file in &self.dropped_files {
                                ui.label(file.as_os_str().to_str().unwrap().to_string());
                            }
                        });
                    }

                    if ui.button("Done").clicked() {
                        self.show_input_files = false;
                    }
                });
        }

        if self.show_invalid_project {
            egui::Window::new("Invalid project folder")
                .collapsible(false)
                .resizable(false)
                .anchor(eframe::emath::Align2::CENTER_CENTER, [0.0, 0.0])
                .show(ctx, |ui| {
                    ui.label("The selected project folder is invalid.");

                    if ui.button("Ok").clicked() {
                        self.show_invalid_project = false;
                    }
                });
        }

        egui::TopBottomPanel::top("my_panel").show(ctx, |ui| {
            egui::menu::bar(ui, |ui| {
                ui.menu_button("File", |ui| {
                    let button = egui::Button::new("New Project");

                    if ui.add_enabled(true, button).clicked() {
                        let file = rfd::FileDialog::new()
                            .set_directory("/")
                            .set_title("New Project")
                            .pick_folder();

                        if let Some(f) = file {
                            let p = Project::new(f);

                            p.setup();

                            self.project = Some(p);
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
                            let p = Project::open_project(f);

                            if p.is_none() {
                                self.show_invalid_project = true;
                            } else {
                                self.project = p;
                            }
                        }

                        ui.close_menu();
                    }

                    let button = egui::Button::new("Input Files");

                    if ui.add_enabled(self.project.is_some(), button).clicked() {
                        self.show_input_files = true;

                        ui.close_menu();
                    }

                    let button = egui::Button::new("Close Project");

                    if ui.add_enabled(self.project.is_some(), button).clicked() {
                        self.project = None;

                        self.selected_object = None;

                        self.open_files = HashSet::new();

                        ui.close_menu();
                    }
                });
            });
        });

        if let Some(f) = &mut self.project {
            let sp = egui::SidePanel::left("my_left_panel").resizable(true);

            let mut open_file = None;

            sp.show(ctx, |ui| {
                egui::ScrollArea::both().show(ui, |ui| {
                    ui.label("Input Files");

                    ui.group(|ui| {
                        for (k, n) in f.infiles.iter() {
                            let r = ui.selectable_label(
                                self.selected_object.eq(&Some(*k)),
                                format!("{}", n),
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
            }
        }

        if let Some(f) = &mut self.project {
            egui::TopBottomPanel::top("file_selection_bar").show(ctx, |ui| {
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

        egui::CentralPanel::default().show(ctx, |ui| {
            egui::ScrollArea::vertical().show(ui, |ui| {
                if let Some(f) = &self.project {
                    if let Some(i) = self.current_object_index {
                        if let Some(file) = f.open_files.get(&i) {
                            ui.label(format!("{}", file.borrow_name()));

                            let obj = file.borrow_obj();

                            for i in object::Object::sections(obj) {
                                ui.label(format!("Section {:?}", i));
                            }
                        }
                    }
                }
            });
        });

        self.preview_files_being_dropped(ctx);

        // Collect dropped files:

        if let Some(prj) = &mut self.project {
            for f in &ctx.input().raw.dropped_files {
                if let Some(p) = &f.path {
                    prj.copy_input_file(p);

                    self.dropped_files.insert(p.clone());
                }
            }
        }
    }
}
