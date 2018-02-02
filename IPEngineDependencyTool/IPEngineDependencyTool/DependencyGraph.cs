using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace IPEngineDependencyTool
{

    public class Dependency
    {
        public Dependency()
        {
            Inject = false;
        }
        public Dependency(Module mod)
        {
            DepModule = mod;
            Inject = true;
        }

        ~Dependency()
        {
            if(m_depmod!=null)
            {
                m_depmod.DependencyCounter--;
            }
        }
        public bool Inject { get; set; }
        public string Identifier { get; set; }
        public Module DepModule { get { return m_depmod; } set { if (m_depmod != null) { m_depmod.DependencyCounter--; }; m_depmod = value; value.DependencyCounter++; } }

        private Module m_depmod;

        public override string ToString()
        {
            return Identifier;
        }
    }

    public class ExPoint
    {
        public ExPoint()
        {
            Extensions = new List<Extension>();
        }
        public string Identifier { get; set; }

        public List<Extension> Extensions {get;set;}

        public override string ToString()
        {
            return Identifier;
        }
    }

    public class Extension
    {
        public Extension(Module mod)
        {
            ExModule = mod;
        }
        public Extension(Module mod, uint p)
        {
            ExModule = mod;
            Priority = p;
        }

        ~Extension()
        {
            if(ExModule!=null)
            {
                ExModule.ExtensionCounter--;
            }
        }
        public Module ExModule { get { return m_mod; } set { if (ExModule != null){m_mod.ExtensionCounter--; };m_mod = value; m_mod.ExtensionCounter++; } }
        public uint Priority { get; set; }

        private Module m_mod;

        public override string ToString()
        {
            return ExModule.Identifier;
        }
    }
    public class Module
    {
        public Module()
        {
            Dependencies = new List<Dependency>();
            ExtensionPoints = new List<ExPoint>();
        }
        public bool Ignore { get; set; }
        public string Identifier { get; set; }
        public bool IsExtension { get; set; }

        public int DependencyCounter { get; set; }
        public int ExtensionCounter { get; set; }
        public List<Dependency> Dependencies { get; set; }
        public List<ExPoint> ExtensionPoints { get; set; }

        public override string ToString()
        {
            return Identifier;
        }
    }

    public class DependencyGraph
    {
        private List<Module> modules = new List<Module>();
        private List<Module> GetRoots()
        {
            return null;
        }

        public void Add(Module m)
        {
            if(!modules.Contains(m))
                modules.Add(m);
        }

        public void Remove(Module m)
        {
            if (modules.Contains(m))
                modules.Remove(m);
        }

        public List<Module> GetModules()
        {
            return modules;
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
