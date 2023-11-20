//! This module covers funcality for custom events within the decompiler on the main event loop

/// The kinds of events that can occur
pub enum EventType {
    /// The network data should be checked
    CheckDecompiler,
}

/// The custom event struct for the application
pub struct Event {
    /// The message sent
    pub message: EventType,
    /// The optional window id for the message
    id: Option<egui_multiwin::winit::window::WindowId>,
}

impl Event {
    /// Return the window id for the custom event
    pub fn window_id(&self) -> Option<egui_multiwin::winit::window::WindowId> {
        self.id
    }

    /// Create a non-window specific event
    pub fn new_general(message: EventType) -> Self {
        Self { message, id: None }
    }
}
