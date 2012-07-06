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
    public partial class Add_Target : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";

        public Add_Target()
        {
            InitializeComponent();

            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            MySqlDataReader Reader;
            command.CommandText = "SELECT serialNum FROM robots";
            connection.Open();
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                string thisrow = "";
                for (int i = 0; i < Reader.FieldCount; i++)
                    thisrow += Reader.GetValue(i).ToString();
                comboBox1.Items.Add(thisrow);
            }
            connection.Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            //Add Target to database
            string targetID = textBox1.Text;
            string taskedRobot = comboBox1.Text;

            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            command.CommandText = "INSERT INTO targets (targetID,taskedRobot,timeIdentified) VALUES (" + targetID + "," + taskedRobot + ",CONCAT(CURDATE(),' ',CURTIME()))";
            connection.Open();
            command.ExecuteNonQuery();
            connection.Close();
            this.Close();
        }
    }
}
