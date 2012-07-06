namespace Command_Center_3
{
    partial class EndEx_Confirm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EndEx_Confirm));
            this.confirmText = new System.Windows.Forms.Label();
            this.Cancel_Button = new System.Windows.Forms.Button();
            this.Confirm_Button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // confirmText
            // 
            this.confirmText.AutoSize = true;
            this.confirmText.Location = new System.Drawing.Point(12, 9);
            this.confirmText.Name = "confirmText";
            this.confirmText.Size = new System.Drawing.Size(217, 13);
            this.confirmText.TabIndex = 8;
            this.confirmText.Text = "Are you sure you want to end the excercise?";
            // 
            // Cancel_Button
            // 
            this.Cancel_Button.Location = new System.Drawing.Point(124, 38);
            this.Cancel_Button.Name = "Cancel_Button";
            this.Cancel_Button.Size = new System.Drawing.Size(75, 23);
            this.Cancel_Button.TabIndex = 7;
            this.Cancel_Button.Text = "Cancel";
            this.Cancel_Button.UseVisualStyleBackColor = true;
            this.Cancel_Button.Click += new System.EventHandler(this.Cancel_Button_Click_1);
            // 
            // Confirm_Button
            // 
            this.Confirm_Button.Location = new System.Drawing.Point(43, 38);
            this.Confirm_Button.Name = "Confirm_Button";
            this.Confirm_Button.Size = new System.Drawing.Size(75, 23);
            this.Confirm_Button.TabIndex = 6;
            this.Confirm_Button.Text = "Confirm";
            this.Confirm_Button.UseVisualStyleBackColor = true;
            this.Confirm_Button.Click += new System.EventHandler(this.Confirm_Button_Click_1);
            // 
            // EndEx_Confirm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(251, 75);
            this.Controls.Add(this.confirmText);
            this.Controls.Add(this.Cancel_Button);
            this.Controls.Add(this.Confirm_Button);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "EndEx_Confirm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Confirm End Exercise";
            this.TopMost = true;
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label confirmText;
        private System.Windows.Forms.Button Cancel_Button;
        private System.Windows.Forms.Button Confirm_Button;
    }
}