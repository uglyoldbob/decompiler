//! Contains the autohashmap definition and implementation.

use std::collections::HashMap;
use std::ops::Index;

#[derive(Clone)]
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
