namespace Command_Center_3
{
    partial class Clear_Database_Confirm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Clear_Database_Confirm));
            this.Confirm_Button = new System.Windows.Forms.Button();
            this.Cancel_Button = new System.Windows.Forms.Button();
            this.confirmText = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // Confirm_Button
            // 
            this.Confirm_Button.Location = new System.Drawing.Point(46, 40);
            this.Confirm_Button.Name = "Confirm_Button";
            this.Confirm_Button.Size = new System.Drawing.Size(75, 23);
            this.Confirm_Button.TabIndex = 0;
            this.Confirm_Button.Text = "Confirm";
            this.Confirm_Button.UseVisualStyleBackColor = true;
            this.Confirm_Button.Click += new System.EventHandler(this.Confirm_Click);
            // 
            // Cancel_Button
            // 
            this.Cancel_Button.Location = new System.Drawing.Point(127, 40);
            this.Cancel_Button.Name = "Cancel_Button";
            this.Cancel_Button.Size = new System.Drawing.Size(75, 23);
            this.Cancel_Button.TabIndex = 1;
            this.Cancel_Button.Text = "Cancel";
            this.Cancel_Button.UseVisualStyleBackColor = true;
            this.Cancel_Button.Click += new System.EventHandler(this.Cancel_Click);
            // 
            // confirmText
            // 
            this.confirmText.AutoSize = true;
            this.confirmText.Location = new System.Drawing.Point(15, 11);
            this.confirmText.Name = "confirmText";
            this.confirmText.Size = new System.Drawing.Size(221, 13);
            this.confirmText.TabIndex = 2;
            this.confirmText.Text = "Are you sure you want to clear the database?";
            // 
            // Clear_Database_Confirm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(251, 75);
            this.Controls.Add(this.confirmText);
            this.Controls.Add(this.Cancel_Button);
            this.Controls.Add(this.Confirm_Button);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Clear_Database_Confirm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Confirm Clear Database";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button Confirm_Button;
        private System.Windows.Forms.Button Cancel_Button;
        private System.Windows.Forms.Label confirmText;
    }
}