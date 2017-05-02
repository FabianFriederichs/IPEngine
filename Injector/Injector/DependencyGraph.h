 #include <string>
 namespace  DependencyGraphStuff{
	 struct Module
	 {
		vector<pair<string, Module> dependencies;
		vector<ExtensionPoint> extensions;
		string identifier;
		string iname;
	 };
	 
	 struct ExtensionPoint
	 { 
		string extensionpointidentifier;
		int position;
		string extensionidentifier;
	 };
	 
	 class DependencyGraph
	 {
	 private:
		vector<Module> roots;
	 public:
		addModule(const Module &m) {roots.push_back(m);}
	 };
 }
 