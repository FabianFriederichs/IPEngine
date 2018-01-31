using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;


namespace IPEngineDependencyTool
{                        
    static class DependencyParser
    {                    
        public static void WriteGraphToXML(DependencyGraph input, string path)
        {
            //TODO
            try
            {
                using (XmlWriter xwriter = XmlWriter.Create(path))
                {
                    XElement xTree = new XElement("DependencyGraph");
                    //build XElement tree
                    {
                        
                        var modulesnode = new XElement("Modules");
                        
                        foreach(var module in input.GetModules())
                        {
                            modulesnode.Add(XElementFromModule(module));
                        }
                        xTree.Add(modulesnode);
                    }

                    xTree.Save(xwriter);
                }
            }
            catch(Exception ex)
            {

            }
        }

        private static XElement XElementFromModule(Module module)
        {
            XElement moduleroot = new XElement("Module");
            XElement ident = new XElement("identifier");
            XElement ignore = new XElement("ignore");
            XElement isexp = new XElement("isExPoint");

            ident.Value = module.Identifier;
            ignore.Value = module.Ignore.ToString().ToLower();
            isexp.Value = module.IsExtension.ToString().ToLower();

            moduleroot.Add(ident);
            moduleroot.Add(ignore);
            moduleroot.Add(isexp);

            XElement dependencies = new XElement("dependencies");
            foreach(var dep in module.Dependencies)
            {
                dependencies.Add(XElementFromDependency(dep));
            }

            XElement extensionponts = new XElement("ExtensionPoints");
            foreach(var exp in module.ExtensionPoints)
            {
                extensionponts.Add(XElementFromExPoint(exp));
            }

            moduleroot.Add(dependencies);
            moduleroot.Add(extensionponts);
            return moduleroot;
        }

        private static XElement XElementFromExPoint(ExPoint exp)
        {
            XElement ExPointroot = new XElement("Point");
            XElement ident = new XElement("identifier");
            ident.Value = exp.Identifier;
            ExPointroot.Add(ident);
            XElement extensions = new XElement("Extensions");
            
            foreach(var ext in exp.Extensions)
            {
                XElement xext = new XElement("Extension");
                XElement name = new XElement("extensionname");
                name.Value = ext.ExModule.Identifier;
                XElement prio = new XElement("priority");
                prio.Value = ext.Priority.ToString();
                xext.Add(name);
                xext.Add(prio);
                extensions.Add(xext);
            }
            ExPointroot.Add(extensions);
            return ExPointroot;
        }

        private static XElement XElementFromDependency(Dependency dep)
        {
            XElement dependencyroot = new XElement("dependency");
            XElement ident = new XElement("identifier");
            XElement inject = new XElement("inject");
            XElement moduleid = new XElement("moduleID");

            ident.Value = dep.Identifier;
            inject.Value = dep.Inject.ToString().ToLower();
            moduleid.Value = dep.DepModule.Identifier;

            dependencyroot.Add(ident);
            dependencyroot.Add(inject);
            dependencyroot.Add(moduleid);
            return dependencyroot;
        }

        public static DependencyGraph ReadGraphFromXML(string path)
        {                
            try          
            {            
                using (XmlReader xreader = XmlReader.Create(path))
                {        

                    DependencyGraph graph = new DependencyGraph();
                    XElement xTree = XElement.Load(xreader);
                    
                    if(xTree.Name == "DependencyGraph")
                    {
                        var e = xTree.Element("Modules");
                        var lxe = e.Descendants("Module").ToList();
                        foreach (var c in lxe)
                        {
                            //lxe are all <Module> nodes
                            //TODO: First create all modules THEN parse dependencies and expoints
                            //Change parsemodulefromxelement to without deps and expoints
                            //then iterate again and call one that only does deps and expoints
                            Module mod = new Module();
                            if (c.Name == "Module")
                            {
                                ParseModuleFromXElement(c, mod);
                                graph.Add(mod);
                            }
                        }
                        int counter = 0;
                        foreach (var c in lxe)
                        {
                            if (c.Name == "Module")
                                ParseModuleFromXElement(c, graph.GetModules()[counter], true, graph.GetModules());
                            counter++;
                        }
                    }
                    return graph;
                }
            }
            catch(Exception ex)
            {
                throw new Exception("Failed to read XML file", ex);
            }
            //return null;
        }

        private static Module ParseModuleFromXElement(XElement xe, Module newModule, bool fancy = false, List<Module> mods =null)
        {
            if (!fancy)
            {
                var e = xe.Element("ignore");
                if (e != null)
                {
                    newModule.Ignore = bool.TrueString.ToLower() == e.Value.ToLower() ? true : false;
                }
                e = xe.Element("identifier");
                if (e != null)
                {
                    newModule.Identifier = e.Value;
                }
                e = xe.Element("isExPoint");
                if (e != null)
                {
                    newModule.IsExtension = bool.TrueString.ToLower() == e.Value.ToLower() ? true : false;
                }
            }
            if (fancy && mods != null)
            {
                var e = xe.Element("dependencies");
                if (e != null)
                {
                    var lxe = e.Descendants("dependency").ToList();
                    foreach (var c in lxe)
                    {
                        //lxe are all <dependency> nodes
                        if (c.Name == "dependency")
                        {
                            var ar = ParseDepFromXElement(c);
                            //get dependency module
                            var dm = mods.FindIndex((m) => { return m.Identifier == ar[2]; });
                            if (dm == -1)
                            {
                                Console.WriteLine("Skipped dependency \"{0}\" of Module \"{1}\"", ar[2], newModule.Identifier);
                                continue;
                            }
                            Dependency dep = new Dependency(mods[dm]);
                            if (ar[0] != null)
                                dep.Inject = ar[0].ToLower() == bool.TrueString ? true : false;
                            else
                                dep.Inject = false;
                            dep.Identifier = ar[1];
                            newModule.Dependencies.Add(dep);
                        }
                    }
                }

                e = xe.Element("ExtensionPoints");
                if (e != null)
                {
                    var lxe = e.Descendants("Point").ToList();
                    foreach (var c in lxe)
                    {
                        //lxe are all <Point> nodes
                        if (c.Name == "Point")
                            newModule.ExtensionPoints.Add(ParseExPFromXElement(c, mods));
                    }
                }
            }
            if (newModule.Identifier != null && newModule.Identifier != "")
                return newModule;
            return null;
        }

        private static string[] ParseDepFromXElement(XElement xe)
        {
            //Dependency dep = new Dependency(mod);
            string[] ar = new string[3];
            var e = xe.Element("inject");
            if (e != null)
            {
                ar[0] = e.Value;// bool.FalseString.ToLower() == e.Value.ToLower() ? false : true;
            }
            e = xe.Element("identifier");
            if (e != null)
            {
                ar[1] = e.Value;
            }
            e = xe.Element("moduleID");
            if (e != null)
            {
                ar[2] = e.Value;// bool.FalseString.ToLower() == e.Value.ToLower() ? false : true;
            }
            return ar;
        }

        private static ExPoint ParseExPFromXElement(XElement xe, List<Module> mods)
        {
            ExPoint xp = new ExPoint();
            var e = xe.Element("identifier");
            if (e != null)
            {
                xp.Identifier = e.Value;// bool.FalseString.ToLower() == e.Value.ToLower() ? false : true;
            }
            else
                return null;
            e = xe.Element("Extensions");
            var lxe = e.Descendants("Extension").ToList();
            foreach (var c in lxe)
            {
                //lxe are all <Extension> nodes
                if (c.Name == "Extension")
                {
                    var ar = ParseExtFromXElement(c);
                    //get dependency module
                    var dm = mods.FindIndex((m) => { return (m.Identifier == ar[0]&&m.IsExtension); });
                    if (dm == -1)
                    {
                        Console.WriteLine("Skipped Extension \"{0}\" of ExtensionPoint \"{1}\"", ar[0], xp.Identifier);
                        continue;
                    }
                    Extension ext = new Extension(mods[dm]);
                    ext.Priority = ar[1].Length>0?uint.Parse(ar[1]):0;
                    xp.Extensions.Add(ext);
                }
            }
            return xp;
        }

        private static string[] ParseExtFromXElement(XElement xe)
        {
            string[] ar = new string[2];
            var e = xe.Element("extensionname");
            if (e != null)
            {
                ar[0] = e.Value;// bool.FalseString.ToLower() == e.Value.ToLower() ? false : true;
            }
            e = xe.Element("priority");
            if (e != null)
            {
                ar[1] = e.Value;
            }
            return ar;
        }
    }
}
/*
 * <DependencyGraph>
    <Module>
      <ignore>false</ignore>
      <identifier>GraphicsModule</identifier>
      <dependencies>
        <dependency>
          <inject>true</inject>
          <identifier>SCM</identifier>
          <moduleID>SimpleContentModule</moduleID>
        </dependency>
      </dependencies>
      <ExtensionPoints>
        <Point>
          <identifier>PreRender</identifier>
          <Extensions>
            <Extension>
              <extensionname>GraphicsModulePreRenderVR</extensionname>
              <priority>0</priority>
            </Extension>
          </Extensions>
        </Point>
      </ExtensionPoints>
    </Module>
    <Module>
      <ignore>false</ignore>
      <identifier>GraphicsModulePreRenderVR</identifier>
      <isExPoint>true</isExPoint>
      <dependencies>
        <dependency>
          <inject>true</inject>
          <identifier>SCM</identifier>
          <moduleID>SimpleContentModule</moduleID>
        </dependency>
      </dependencies>
</DependencyGraph>
 * */
