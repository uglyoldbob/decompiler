//! This module defines all of the gui windows that might be used in the decompiler.
use egui_multiwin::enum_dispatch::enum_dispatch;

use crate::egui_multiwin_dynamic::tracked_window::{RedrawResponse, TrackedWindow};
use egui_multiwin::egui_glow::EguiGlow;
use std::sync::Arc;

pub mod root;

/// The enum of all possible windows in the decompiler.
#[enum_dispatch(TrackedWindow)]
pub enum MyWindows {
    /// The root window
    Root(root::RootWindow),
}
