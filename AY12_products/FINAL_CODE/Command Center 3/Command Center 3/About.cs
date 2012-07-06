using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Command_Center_3
{
    public partial class About : Form
    {
        public About()
        {
            InitializeComponent();
            richTextBox1.Clear();
            richTextBox1.SelectionAlignment = HorizontalAlignment.Center;
            richTextBox1.SelectedText = "Command Center allows operators to monitor air, ground, and water robots during a mission.\n\nThis is part of the Joint Cooperative Unmanned Systems Initiative (JCUSI) capstone project.\n\nAdvisors:\nCOL Robert Sadowski\nLTC Scott Lathrop\nLTC Robert McTasney\nMAJ Dominic Larkin\n\nTeam Members:\nCDT David Burkhardt (CS)\nCDT Bradley Campbell (EE)\nCDT Bryce Martens (EE)\nCDT Rafael Garcia-Menocal (CS)\nCDT Myea Rice (IT)\nCDT Rnobert Ryan (EE)\nCDT James Taylor (CS)\nCDT Kamal Wheeler (EE)\nCDT Andrew Wei (CS)\n\n© 2011, United States Military Academy";
        }
        
        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
