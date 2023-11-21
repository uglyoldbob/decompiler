//! Defines code that generates files for the autotools build system

use std::{io::Write, path::PathBuf};

/// The struct for the autotools build system
pub struct BuildSystem {}

impl super::BuildSystemTrait for BuildSystem {
    #[doc = " Write the build files to the filesystem at the specified path"]
    fn write(&self, details: &super::ProjectDetails) -> Result<(), std::io::Error> {
        let mut configure_ac = details.path.clone();
        configure_ac.push("configure.ac");
        let mut f = std::fs::File::create(configure_ac)?;
        f.write_all("AC_PREREQ([2.62])\n".as_bytes())?;
        f.write_all(format!("AC_INIT([{}], [{}])\n", details.name, details.version).as_bytes())?;
        f.write_all("AM_INIT_AUTOMAKE\n".as_bytes())?;
        f.write_all("AC_PROG_CC\n".as_bytes())?;
        f.write_all("AC_CONFIG_HEADER([ac_config.h:config.h.in])\n".as_bytes())?;
        f.write_all("AC_CHECK_TOOL([STRIP],[strip])\n".as_bytes())?;
        f.write_all("AC_CONFIG_FILES([\n".as_bytes())?;
        f.write_all("	Makefile\n".as_bytes())?;
        f.write_all("])\n".as_bytes())?;
        f.write_all("AC_OUTPUT\n".as_bytes())?;
        f.flush()?;
        drop(f);
        Ok(())
    }
}

impl BuildSystem {
    /// Construct a new object
    pub fn new() -> Self {
        Self {}
    }
}
