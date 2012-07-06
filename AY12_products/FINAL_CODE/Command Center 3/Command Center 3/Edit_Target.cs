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
    public partial class Edit_Target : Form
    {
        private string MyConString = "SERVER=localhost;DATABASE=commandcenter_db;UID=root;PASSWORD=abc;";
        private string tID;

        public Edit_Target(string selectedTarget)
        {
            InitializeComponent();

            string defaultRobot = "";
            tID = selectedTarget;
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            MySqlDataReader Reader;
            command.CommandText = "SELECT * FROM (SELECT t.targetID AS ID, tl.latitude AS Latitude, tl.longitude AS Longitude, tl.timeUpdated AS Time_Identified, t.taskedRobot AS Tasked_Robot FROM targets as t INNER JOIN targetlocations as tl ON t.targetID = tl.targetID ORDER BY Time_Identified DESC) AS inter_view WHERE ID = "+ tID +" GROUP BY ID";
            connection.Open();
            Reader = command.ExecuteReader();
            while (Reader.Read())
            {
                textBox1.Text = Reader.GetValue(1).ToString();
                textBox2.Text = Reader.GetValue(2).ToString();
                defaultRobot = Reader.GetValue(4).ToString();
            }
            Reader.Close();

            command.CommandText = "SELECT serialNum FROM robots";
            Reader = command.ExecuteReader();
            int r = 0;
            while (Reader.Read())
            {
                string thisrow = "";
                for (int i = 0; i < Reader.FieldCount; i++)
                {
                    thisrow += Reader.GetValue(i).ToString();
                    comboBox1.Items.Add(thisrow);
                    if (thisrow == defaultRobot)
                        comboBox1.SelectedIndex = r;
                }
                r++;
            }
            connection.Close();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            this.Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            
            MySqlConnection connection = new MySqlConnection(MyConString);
            MySqlCommand command = connection.CreateCommand();
            connection.Open();
            command.CommandText = "UPDATE targets SET taskedRobot = " + comboBox1.SelectedItem.ToString() + " WHERE targetID = " + tID;
            command.ExecuteNonQuery();
            command.CommandText = "INSERT INTO targetlocations (targetID,timeUpdated,latitude,longitude) VALUES (" + tID + ",CONCAT(CURDATE(),' ',CURTIME())," + textBox1.Text.ToString() + "," + textBox2.Text.ToString() + ")";
            command.ExecuteNonQuery();
            connection.Close();
            this.Close();
        }


    }
}
