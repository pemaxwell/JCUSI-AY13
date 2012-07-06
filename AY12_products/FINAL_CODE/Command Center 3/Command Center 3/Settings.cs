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
    public partial class Settings : Form
    {
        public string dbserver = "127.0.0.1";
        public string schema = "commandcenter_db";
        public string user = "root";
        public string password = "abc";

        public Settings()
        {
            InitializeComponent();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            this.dbserver = "127.0.0.1";
            this.schema = "commandcenter_db";
            this.user = "root";
            this.password = "abc";
            this.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.dbserver = textBox1.Text;
            this.schema = textBox2.Text;
            this.user = textBox3.Text;
            this.password = textBox4.Text;
            this.Close();
        }
    }
}
