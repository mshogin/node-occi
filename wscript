import Options, Utils, sys
from os import unlink, symlink, chdir, environ
from os.path import exists

def set_options(opt):
        opt.tool_options("compiler_cxx")
	
def configure(conf):
         
        conf.check_tool("compiler_cxx")
        conf.check_tool("node_addon")

        if not environ['ORACLE_HOME']:
            conf.fatal("not found ORACLE_HOME")
    
        conf.env.append_unique('CXXFLAGS', ["-Wall"])
        conf.env.append_unique('CXXFLAGS', "-I" + environ['ORACLE_HOME'] + "/sdk/include/")	
        
        conf.env.append_unique('LINKFLAGS', "-locci")	
        conf.env.append_unique('LINKFLAGS', "-lclntsh")
        conf.env.append_unique('LINKFLAGS', "-L" +  environ['ORACLE_HOME'] )

        if not conf.check(header_name="occi.h"):
            conf.fatal("not found occi.h")

        if not conf.check_cxx(lib='occi'):
            conf.fatal("not found libocci")

    
    
def build(bld):
        obj = bld.new_task_gen("cxx", "shlib", "node_addon") 
        obj.target = "node_occi"
        obj.source = "./src/bind.cpp ./src/conn.cpp ./src/st.cpp ./src/rs.cpp"
        obj.uselib = "OCCI"
