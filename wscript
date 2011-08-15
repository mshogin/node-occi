import Options, Utils
from os import unlink, symlink, chdir
from os.path import exists


def set_options(opt):
	opt.tool_options("compiler_cxx")
	opt.tool_options("compiler_cxx")
	
def configure(conf):
	conf.check_tool("compiler_cxx")
	conf.check_tool("node_addon")

 	
	conf.env.append_unique('CXXFLAGS', ["-Wall"])
	conf.env.append_unique('CXXFLAGS', "-I/usr/local/instantclient_11_2/sdk/demo/")	
	conf.env.append_unique('CXXFLAGS', "-I/usr/local/instantclient_11_2/sdk/include/")	
	conf.env.append_unique('LINKFLAGS', "-locci")	
	conf.env.append_unique('LINKFLAGS', "-lclntsh")
	conf.env.append_unique('LINKFLAGS', "-L/usr/local/instantclient_11_2/")

	if not conf.check_cxx(lib='occi'):
		conf.fatal("not found libocci")
    
    
def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon") 
  obj.target = "node_oracle"
  obj.source = "./src/bind.cpp ./src/conn.cpp ./src/st.cpp ./src/rs.cpp"
  obj.uselib = "OCCI"
