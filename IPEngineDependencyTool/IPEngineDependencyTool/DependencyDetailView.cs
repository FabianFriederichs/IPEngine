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
    public partial class DependencyDetailView : UserControl
    {
        private Dependency currentDep;
        public DependencyDetailView()
        {
            InitializeComponent();
        }

        public event EventHandler DataChanged;

        public void setDependency(Dependency d, DependencyGraph g, Module m)
        {
            currentDep = d;
            textBox1.Text = d.Identifier;
            checkBox1.Checked = d.Inject;
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(g.GetModules().ToArray());
            comboBox1.Items.Remove(m);
            comboBox1.SelectedItem = d.DepModule;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            currentDep.Identifier = textBox1.Text;
            DataChanged?.Invoke(sender, e);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            currentDep.Inject = checkBox1.Checked;
            DataChanged?.Invoke(sender, e);
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            currentDep.DepModule = (Module)comboBox1.SelectedItem;
            DataChanged?.Invoke(sender, e);
        }
    }
}
