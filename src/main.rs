#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")] // hide console window on Windows in release
#![deny(missing_docs)]
#![deny(clippy::missing_docs_in_private_items)]

//! The decompiler. It attempts to decompile various types of programs into compilable source code.

use egui_multiwin::egui;
use egui_multiwin_dynamic::multi_window::NewWindowRequest;

/// Dynamically generated code from the egui-multiwin crate
pub mod egui_multiwin_dynamic {
    egui_multiwin::tracked_window!(crate::MyApp, crate::event::Event, crate::windows::MyWindows);
    egui_multiwin::multi_window!(crate::MyApp, crate::event::Event, crate::windows::MyWindows);
}

mod block;
mod decompiler;
mod event;
mod windows;

use std::collections::HashMap;
use std::io::Read;
use std::ops::Index;
use std::path::PathBuf;
use std::sync::Arc;

use ouroboros::self_referencing;

fn main() {
    egui_multiwin_dynamic::multi_window::MultiWindow::start(|multi_window, event_loop, _proxy| {
        let root_window = windows::root::RootWindow::request();

        let mut ac: MyApp = MyApp::default();

        if let Err(e) = multi_window.add(root_window, &mut ac, event_loop) {
            println!("Failed to create main window {:?}", e);
        }
        ac
    });
}

/// A HashMap implementation that auto-indexes contents. Behaves somewhat like a `Vec<T>`.
pub struct AutoHashMap<T> {
    /// The data contained in the map
    d: HashMap<usize, T>,
    /// Used for generating the next index when inserting into the map
    next: usize,
}

impl<T> AutoHashMap<T> {
    /// Generate a new object.
    pub fn new() -> Self {
        Self {
            d: HashMap::new(),
            next: 0,
        }
    }

    /// Return an mutable iterator over the HashMap contained within
    pub fn iter_mut(&mut self) -> std::collections::hash_map::IterMut<'_, usize, T> {
        self.d.iter_mut()
    }

    /// Return an iterator over the HashMap contained within
    pub fn iter(&self) -> std::collections::hash_map::Iter<'_, usize, T> {
        self.d.iter()
    }

    /// Return the length of the map
    pub fn len(&self) -> usize {
        self.d.len()
    }

    /// Get a particular element of the map, if it exists.
    pub fn get(&self, k: &usize) -> Option<&T> {
        self.d.get(k)
    }

    /// Insert an element into the map, automatically assigning an index to it.
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

/// A decompile project
pub struct Project {
    /// The folder path for the project.
    pub path: PathBuf,
    /// The input file path in the project.
    pub inputs: PathBuf,
    /// The input files for the project.s
    pub infiles: AutoHashMap<String>,
    /// The list of open files for the project.
    pub open_files: HashMap<usize, Arc<ProjectInputFile>>,
    /// The decompiler object for the project
    decompiler: crate::decompiler::Decompiler,
}

impl Project {
    /// Create a new object
    pub fn new(p: PathBuf) -> Self {
        let mut inp = p.clone();
        inp.push("input");
        Self {
            path: p.clone(),
            inputs: inp,
            infiles: AutoHashMap::new(),
            open_files: HashMap::new(),
            decompiler: crate::decompiler::Decompiler::new(p),
        }
    }

    /// Open an existing project, if it exists, by specifying the path.
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

    /// Create the project foler
    pub fn setup(&self) {
        std::fs::create_dir(&self.inputs);
    }

    /// Copy a file to the input folder of the project.
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

    /// Open a file with the given index
    pub fn open_file(&mut self, index: usize) -> Result<(), ()> {
        if let std::collections::hash_map::Entry::Vacant(e) = self.open_files.entry(index) {
            if let Some(name) = self.infiles.get(&index) {
                let mut fp = self.inputs.clone();
                fp.push(name);
                let f = std::fs::File::open(fp);
                if let Ok(mut file) = f {
                    let mut v = Vec::new();
                    file.read_to_end(&mut v);
                    let nf = ProjectInputFileBuilder {
                        data: v,
                        obj_builder: |a| object::File::parse(&a[..]).unwrap(),
                        name: name.clone(),
                        f: file,
                    }
                    .build();
                    e.insert(Arc::new(nf));
                    Ok(())
                } else {
                    Err(())
                }
            } else {
                Err(())
            }
        } else {
            Err(())
        }
    }

    /// Trigger parsing of the file with the given index, file must already be open
    pub fn trigger_parse_file(&mut self, index: usize) {
        let f = self.open_files.get(&index);
        if let Some(f) = f {
            self.decompiler.process_file(f);
        }
    }

    /// Write all decompilation outputs to disk.
    pub fn write_outputs(&self) {
        self.decompiler.write_outputs();
    }

    /// Close the file with the given index
    pub fn close_file(&mut self, index: usize) {
        if !self.open_files.contains_key(&index) {
            if let Some(_name) = self.infiles.get(&index) {
                self.open_files.remove(&index);
            }
        }
    }
}

/// The main shared object for the decompiler.
#[derive(Default)]
pub struct MyApp {
    /// The project for the decompiler
    project: Option<Project>,
}

impl MyApp {
    /// Process events received on the gui event loop
    fn process_event(&mut self, event: crate::event::Event) -> Vec<NewWindowRequest> {
        let windows_to_create = vec![];
        match event.message {
            crate::event::EventType::CheckDecompiler => {
                if let Some(p) = &mut self.project {
                    p.decompiler.process_events();
                }
            }
        }
        windows_to_create
    }

    /// Used to receive files with drag and drop.
    fn preview_files_being_dropped(&mut self, ctx: &egui::Context) {
        use egui::*;

        use std::fmt::Write as _;

        let hf = ctx.input(|r| r.raw.hovered_files.clone());
        if !hf.is_empty() {
            let mut text = "Dropping files:\n".to_owned();

            for file in hf {
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

            let screen_rect = ctx.input(|r| r.screen_rect);

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
