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
    public partial class ExtensionDetailView : UserControl
    {
        private ExPoint currentExp;
        public ExtensionDetailView()
        {
            InitializeComponent();
        }

        public event EventHandler DataChanged;

        public void SetExPoint(ExPoint e, DependencyGraph g)
        {
            currentExp = e;
            textBox1.Text = e.Identifier;
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(g.GetModules().FindAll((m) => { return m.IsExtension; }).ToArray());
            updateListBox();
        }

        private void tableLayoutPanel1_Paint(object sender, PaintEventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            currentExp.Identifier = textBox1.Text;
            DataChanged?.Invoke(sender, e);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void comboBox1_SelectionChangeCommitted(object sender, EventArgs e)
        {
            //listBox1.Items.Add(comboBox1.SelectedItem);
            currentExp.Extensions.Add(new Extension((Module)comboBox1.SelectedItem, (uint)listBox1.Items.Count)); //dirty
            //listBox1.Items.Add(currentExp.Extensions.Last());
            updateListBox();
            DataChanged?.Invoke(sender, e);
        }

        private void listBox1_DragDrop(object sender, DragEventArgs e)
        {
            Point point = listBox1.PointToClient(new Point(e.X, e.Y));
            int index = this.listBox1.IndexFromPoint(point);
            if (index < 0) index = this.listBox1.Items.Count - 1;
            object data = e.Data.GetData(typeof(Extension));
            this.listBox1.Items.Remove(data);
            this.listBox1.Items.Insert(index, data);
            ((Extension)data).Priority = index>0?(uint)index:0;
        }

        private void listBox1_MouseDown(object sender, MouseEventArgs e)
        {
            if (this.listBox1.SelectedItem == null) return;
            this.listBox1.DoDragDrop(this.listBox1.SelectedItem, DragDropEffects.Move);
        }

        private void listBox1_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.Move;
        }

        private void listBox1_KeyUp(object sender, KeyEventArgs e)
        {
            if ((e.Control && e.KeyCode == Keys.D) || e.KeyCode == Keys.Delete)
            {
                //remove
                currentExp.Extensions.Remove((Extension)listBox1.SelectedItem);
                updateListBox();
            }
        }
        private void updateListBox()
        {
            var s = listBox1.SelectedItem;
            listBox1.Items.Clear();
            listBox1.Items.AddRange(currentExp.Extensions.ToArray());
            listBox1.SelectedItem = s;
        }
    }
}
