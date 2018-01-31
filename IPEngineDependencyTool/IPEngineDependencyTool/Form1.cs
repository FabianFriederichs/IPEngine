using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace IPEngineDependencyTool
{
    public partial class Form1 : Form
    {

        BindingList<Dependency> depbindlist;
        BindingList<ExPoint> expbindlist;
        public Form1()
        {
            InitializeComponent();
            depGraphControl1.ModuleSelectionChanged += DepGraphControl1_ModuleSelectionChanged;
            moduleDetailView1.DataChanged += ModuleDetailView1_DataChanged;
            dependencyDetailView1.DataChanged += DependencyDetailView1_DataChanged;
            extensionDetailView1.DataChanged += ExtensionDetailView1_DataChanged;
            depGraphControl1.DepGraph = new DependencyGraph();
        }

        private void ExtensionDetailView1_DataChanged(object sender, EventArgs e)
        {
            expbindlist.ResetBindings();
        }

        private void DependencyDetailView1_DataChanged(object sender, EventArgs e)
        {
            depbindlist.ResetBindings();

        }

        private void ModuleDetailView1_DataChanged(object sender, EventArgs e)
        {
            depGraphControl1.ResetBinding();
        }

        private void DepGraphControl1_ModuleSelectionChanged(object sender, EventArgs e)
        {
            var m = depGraphControl1.getSelected();
            //listBox1.Items.Clear();
            //listBox2.Items.Clear();
            //listBox1.ClearSelected();
            //listBox2.ClearSelected();
            //listBox1.Items.AddRange(m.Dependencies.ToArray());
            //listBox2.Items.AddRange(m.ExtensionPoints.ToArray());


            resetBindings(m);
            listBox1.ClearSelected();
            listBox2.ClearSelected();
            moduleDetailView1.SetModule(m);
            ModuleDetailVisible();
        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            var d = (Dependency)listBox1.SelectedItem;
            if (d != null)
            {
                listBox2.ClearSelected();
                dependencyDetailView1.setDependency(d, depGraphControl1.DepGraph, depGraphControl1.getSelected());
                DepDetailVisible();
            }
        }

        private void listBox2_SelectedIndexChanged(object sender, EventArgs e)
        {
            var d = (ExPoint)listBox2.SelectedItem;
            if (d != null)
            {
                listBox1.ClearSelected();
                extensionDetailView1.SetExPoint(d, depGraphControl1.DepGraph);
                ExtDetailVisible();
            }
        }

        private void cmdOpenXML_Click(object sender, EventArgs e)
        {
            ofdOpenXML.ShowDialog();
            var fname = ofdOpenXML.FileName;
            DependencyGraph graph;
            if(File.Exists(fname))
            {
                graph = DependencyParser.ReadGraphFromXML(fname);
                depGraphControl1.DepGraph = graph;
                lblFileName.Text = fname;
            }
        }
        private void ModuleDetailVisible()
        {
            extensionDetailView1.Visible = false;
            dependencyDetailView1.Visible = false;
            moduleDetailView1.Visible = true;
        }

        private void DepDetailVisible()
        {
            extensionDetailView1.Visible = false;
            dependencyDetailView1.Visible = true;
            moduleDetailView1.Visible = false;
        }

        private void ExtDetailVisible()
        {
            extensionDetailView1.Visible = true;
            dependencyDetailView1.Visible = false;
            moduleDetailView1.Visible = false;
        }

        private void depGraphControl1_Enter(object sender, EventArgs e)
        {
            ModuleDetailVisible();
        }
        
        private void resetBindings(Module m)
        {
            depbindlist = new BindingList<Dependency>(m.Dependencies);
            expbindlist = new BindingList<ExPoint>(m.ExtensionPoints);

            listBox1.DataSource = depbindlist;
            listBox1.DisplayMember = "Identifier";

            listBox2.DataSource = expbindlist;
            listBox2.DisplayMember = "Identifier";
        }

        private void listBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            
        }

        private void listBox1_KeyUp(object sender, KeyEventArgs e)
        {
            if(e.Control && e.KeyCode == Keys.A)
            {
                //add
                var d = new Dependency();
                d.Identifier = "NewDep";
                d.Inject = true;
                depbindlist.Add(d);
                resetBindings(depGraphControl1.getSelected());
                listBox1.SelectedItem = d;
            }
            else if(e.Control && e.KeyCode == Keys.D)
            {
                //remove
                depbindlist.Remove((Dependency)listBox1.SelectedItem);
                resetBindings(depGraphControl1.getSelected());
            }
        }

        private void listBox2_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Control && e.KeyCode == Keys.A)
            {
                //add
                var d = new ExPoint();
                d.Identifier = "NewExP";
                expbindlist.Add(d);
                resetBindings(depGraphControl1.getSelected());
                listBox2.SelectedItem = d;
            }
            else if ((e.Control && e.KeyCode == Keys.D) || e.KeyCode == Keys.Delete)
            {
                //remove
                expbindlist.Remove((ExPoint)listBox2.SelectedItem);
                resetBindings(depGraphControl1.getSelected());
            }
        }

        private void cmdSave_Click(object sender, EventArgs e)
        {
            if(lblFileName.Text!="")
            {
                //shop dialog for override
                var res = MessageBox.Show("Do you wanna override?", "OVERRIDE", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
                if(res==DialogResult.Yes)
                {
                    //yes
                    if (File.Exists(lblFileName.Text))
                    {
                        DependencyParser.WriteGraphToXML(depGraphControl1.DepGraph, lblFileName.Text);
                    }
                }
                else if(res==DialogResult.No)
                {
                    //show save path dialog
                    if(sfdSaveXML.ShowDialog() == DialogResult.OK)
                    {
                        DependencyParser.WriteGraphToXML(depGraphControl1.DepGraph, sfdSaveXML.FileName);
                    }
                }
                else
                {
                    //cancel
                }
            }
            else
            {
                if (sfdSaveXML.ShowDialog() == DialogResult.OK)
                {
                    DependencyParser.WriteGraphToXML(depGraphControl1.DepGraph, sfdSaveXML.FileName);
                }

            }
        }
    }
}
