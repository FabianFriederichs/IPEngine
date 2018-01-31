using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IPEngineDependencyTool
{
    public partial class DepGraphControl : UserControl
    {
        DependencyGraph m_depgraph;
        Module lastSelectedModule;
        BindingList<Module> items;
        public DepGraphControl()
        {
            InitializeComponent();
        }

        public event EventHandler ModuleSelectionChanged;

        public Module getSelected() { return lastSelectedModule; }

        public DependencyGraph DepGraph
        {
            get
            {
                return m_depgraph;
            }
            set
            {
                if (value == null)
                    return;
                m_depgraph = value;

                //listBox1.Items.Clear();

                //foreach(var i in m_depgraph.GetModules())
                //{
                //    listBox1.Items.Add(i);
                //}
                items = new BindingList<Module>(m_depgraph.GetModules());
                listBox1.DataSource = items;
                listBox1.DisplayMember = "Identifier";
            }
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            //TreeNode node = e.Node;
            //while (node.Parent != null)
            //{
            //    node = node.Parent;
            //}
            //if (lastSelectedRootNode==null)
            //{
            //    lastSelectedRootNode = node;
            //    if(this.ModuleSelectionChanged!=null)
            //    {
            //        this.ModuleSelectionChanged(this, EventArgs.Empty); //make a custom eventargs to return the selected module with
            //    }
            //}
            //else if(lastSelectedRootNode!=node)
            //{
            //    lastSelectedRootNode = node;
            //    if (this.ModuleSelectionChanged != null)
            //    {
            //        this.ModuleSelectionChanged(this, EventArgs.Empty); //make a custom eventargs to return the selected module with
            //    }
            //}
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            if(listBox1.SelectedItem!=lastSelectedModule)
            {
                lastSelectedModule = (Module)listBox1.SelectedItem;
                ModuleSelectionChanged(this, EventArgs.Empty);
            }
            items.ResetBindings();
        }

        public void ResetBinding()
        {
            items.ResetBindings();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var m = new Module();
            m.Identifier = "NewModule";
            DepGraph.Add(m);
            ResetBinding();
            listBox1.SelectedItem = m;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            DepGraph.Remove((Module)listBox1.SelectedItem);
            ResetBinding();
        }
    }
}
