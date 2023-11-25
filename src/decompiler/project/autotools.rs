//! Defines code that generates files for the autotools build system

use std::io::Write;

use crate::decompiler::FileResults;

/// The struct for the autotools build system
pub struct BuildSystem {}

impl super::BuildSystemTrait for BuildSystem {
    #[doc = " Write the build files to the filesystem at the specified path"]
    fn write(
        &self,
        details: &super::ProjectDetails,
        o: &Vec<FileResults>,
    ) -> Result<(), std::io::Error> {
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

        let mut makefile_am = details.path.clone();
        makefile_am.push("Makefile.am");
        let mut f = std::fs::File::create(makefile_am)?;
        f.write_all("AUTOMAKE_OPTIONS = subdir-objects\n".as_bytes())?;
        if o.len() > 0 {
            f.write_all("bin_PROGRAMS = ".as_bytes())?;
            for file in o {
                let name = file.name.clone();
                f.write_all(format!("\\\n {}", name).as_bytes())?;
            }
            f.write_all("\n".as_bytes())?;

            for file in o {
                f.write_all("\n".as_bytes())?;
                let name = file.name.clone();
                f.write_all(format!("{}_SOURCES = ", name).as_bytes())?;
                for src in &file.sources {
                    f.write_all(format!("\\\n src/{}", src.name).as_bytes())?;
                }
                f.write_all("\n".as_bytes())?;
            }
        }
        f.flush()?;
        drop(f);

        for file in o {
            let mut pb = details.path.clone();
            pb.push("src");
            std::fs::create_dir_all(&pb)?;
            for src in &file.sources {
                src.write_dots(&pb);
                let mut pb = pb.clone();
                pb.push(format!("{}", src.name));
                println!("Creating source file {}", pb.display());
                let f = std::fs::File::create(pb)?;
                src.write_source(f)?;
            }
        }

        Ok(())
    }
}

impl BuildSystem {
    /// Construct a new object
    pub fn new() -> Self {
        Self {}
    }
}
