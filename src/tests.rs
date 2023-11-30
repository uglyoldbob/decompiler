#[test]
fn test_blocks_3() {
    let gg = crate::generator::GraphGenerator::new(3);
    let gi = gg.create_iter();
    let pb = std::path::PathBuf::from("./blocktest");
    let project = crate::decompiler::project::Project::new(
        crate::decompiler::project::autotools::BuildSystem::new().into(),
        pb,
    );
    let mut sf = crate::decompiler::SourceFile::new("blocks3.c".to_string());
    for (i, g) in gi.enumerate() {
        let gb = crate::block::Graph::<crate::block::Block>::from(g);
        let mut dot = Vec::new();
        gb.write_to_dot("asdf", &mut dot).unwrap();
        let fname = format!("f{:X}", i);
        let function = crate::decompiler::Function::new(fname, Vec::new(), gb, dot);
        sf.add_function(function);
    }
    let mut fr = crate::decompiler::FileResults::new("blocktest".to_string());
    fr.add_source(sf);
    let mut vfr = Vec::new();
    vfr.push(fr);
    project.write(&vfr).unwrap();
}
