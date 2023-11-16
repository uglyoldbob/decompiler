#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")] // hide console window on Windows in release

use egui_multiwin::egui;
use egui_multiwin_dynamic::multi_window::NewWindowRequest;

pub mod egui_multiwin_dynamic {
    egui_multiwin::tracked_window!(
        crate::MyApp,
        egui_multiwin::NoEvent,
        crate::windows::MyWindows
    );
    egui_multiwin::multi_window!(
        crate::MyApp,
        egui_multiwin::NoEvent,
        crate::windows::MyWindows
    );
}

mod windows;

use std::collections::HashMap;
use std::collections::HashSet;
use std::io::Read;
use std::ops::Index;
use std::path::PathBuf;

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

pub struct MyApp {
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
    fn process_event(&mut self, event: egui_multiwin::NoEvent) -> Vec<NewWindowRequest> {
        let mut windows_to_create = vec![];
        windows_to_create
    }
}
