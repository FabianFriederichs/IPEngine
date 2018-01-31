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
    public partial class ModuleDetailView : UserControl
    {
        private Module currentModule;
        public ModuleDetailView()
        {
            InitializeComponent();
        }

        public event EventHandler DataChanged;

        public void SetModule(Module m)
        {
            currentModule = m;
            textBox1.Text = m.Identifier;
            checkBox1.Checked = m.Ignore;
            checkBox2.Checked = m.IsExtension;
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            currentModule.Identifier = textBox1.Text;
            DataChanged?.Invoke(sender, e);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            currentModule.Ignore = checkBox1.Checked;
            DataChanged?.Invoke(sender, e);
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            currentModule.IsExtension = checkBox2.Checked;
            DataChanged?.Invoke(sender, e);
        }
    }
}
