using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MySql.Data.MySqlClient;

namespace Command_Center_3
{

    public partial class Register_Robot : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";

        public Register_Robot()
        {
            InitializeComponent();

            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            MySqlDataReader Reader;
            command.CommandText = "SELECT type FROM robottypelookup";
            connection.Open();
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                comboBox1.Items.Add(Reader.GetValue(0).ToString());
            }
            connection.Close();

        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            string serialNum = textBox1.Text;
            string ipaddr = textBox2.Text;
            string robotType = comboBox1.Text;
           
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            command.CommandText = "INSERT INTO robots (serialNum,ipaddr,robotType,currentMission) VALUES ("+serialNum+",'" +ipaddr+"',(SELECT typeID FROM robottypelookup WHERE type = '"+robotType+"'),1)";
            connection.Open();
            try
            {
                command.ExecuteNonQuery();
            }
            catch (MySqlException mse)
            {
                MessageBox.Show("Cannot add robot. Serial Number already exists.");
            }
            connection.Close();
            this.Close();
        }

    }
}
