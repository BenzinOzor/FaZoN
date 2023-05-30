namespace FileEncrypter.Code
{
	partial class ProjectSelector
	{
		/// <summary>
		///  Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		///  Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		///  Required method for Designer support - do not modify
		///  the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectSelector));
			this.ListView_Projects = new System.Windows.Forms.ListView();
			this.columnHeaderProjectName = new System.Windows.Forms.ColumnHeader();
			this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.TextBox_ProjectName = new System.Windows.Forms.TextBox();
			this.TextBox_ProjectPath = new System.Windows.Forms.TextBox();
			this.Button_ProjectPath = new System.Windows.Forms.Button();
			this.Button_New = new System.Windows.Forms.Button();
			this.Button_Delete = new System.Windows.Forms.Button();
			this.Button_OK = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// ListView_Projects
			// 
			this.ListView_Projects.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeaderProjectName,
            this.columnHeader1});
			this.ListView_Projects.FullRowSelect = true;
			this.ListView_Projects.HideSelection = false;
			this.ListView_Projects.Location = new System.Drawing.Point(12, 12);
			this.ListView_Projects.MultiSelect = false;
			this.ListView_Projects.Name = "ListView_Projects";
			this.ListView_Projects.Size = new System.Drawing.Size(776, 334);
			this.ListView_Projects.TabIndex = 0;
			this.ListView_Projects.UseCompatibleStateImageBehavior = false;
			this.ListView_Projects.View = System.Windows.Forms.View.Details;
			this.ListView_Projects.SelectedIndexChanged += new System.EventHandler(this.ListView_Projects_SelectedIndexChanged);
			this.ListView_Projects.DoubleClick += new System.EventHandler(this.ListView_Projects_DoubleClick);
			// 
			// columnHeaderProjectName
			// 
			this.columnHeaderProjectName.Text = "Project";
			this.columnHeaderProjectName.Width = 200;
			// 
			// columnHeader1
			// 
			this.columnHeader1.Text = "Path";
			this.columnHeader1.Width = 572;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(13, 355);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(44, 15);
			this.label1.TabIndex = 1;
			this.label1.Text = "Project";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(13, 385);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(31, 15);
			this.label2.TabIndex = 2;
			this.label2.Text = "Path";
			// 
			// TextBox_ProjectName
			// 
			this.TextBox_ProjectName.Location = new System.Drawing.Point(63, 352);
			this.TextBox_ProjectName.Name = "TextBox_ProjectName";
			this.TextBox_ProjectName.Size = new System.Drawing.Size(725, 23);
			this.TextBox_ProjectName.TabIndex = 3;
			this.TextBox_ProjectName.TextChanged += new System.EventHandler(this.TextBox_ProjectName_TextChanged);
			// 
			// TextBox_ProjectPath
			// 
			this.TextBox_ProjectPath.Location = new System.Drawing.Point(63, 382);
			this.TextBox_ProjectPath.Name = "TextBox_ProjectPath";
			this.TextBox_ProjectPath.Size = new System.Drawing.Size(693, 23);
			this.TextBox_ProjectPath.TabIndex = 4;
			this.TextBox_ProjectPath.TextChanged += new System.EventHandler(this.TextBox_ProjectPath_TextChanged);
			this.TextBox_ProjectPath.KeyUp += new System.Windows.Forms.KeyEventHandler(this.TextBox_ProjectPath_KeyUp);
			this.TextBox_ProjectPath.Validated += new System.EventHandler(this.TextBox_ProjectPath_Validated);
			// 
			// Button_ProjectPath
			// 
			this.Button_ProjectPath.Location = new System.Drawing.Point(762, 381);
			this.Button_ProjectPath.Name = "Button_ProjectPath";
			this.Button_ProjectPath.Size = new System.Drawing.Size(27, 24);
			this.Button_ProjectPath.TabIndex = 5;
			this.Button_ProjectPath.Text = "...";
			this.Button_ProjectPath.UseVisualStyleBackColor = true;
			this.Button_ProjectPath.Click += new System.EventHandler(this.Button_ProjectPath_Click);
			// 
			// Button_New
			// 
			this.Button_New.Location = new System.Drawing.Point(13, 415);
			this.Button_New.Name = "Button_New";
			this.Button_New.Size = new System.Drawing.Size(75, 23);
			this.Button_New.TabIndex = 6;
			this.Button_New.Text = "New";
			this.Button_New.UseVisualStyleBackColor = true;
			this.Button_New.Click += new System.EventHandler(this.Button_New_Click);
			// 
			// Button_Delete
			// 
			this.Button_Delete.Location = new System.Drawing.Point(94, 415);
			this.Button_Delete.Name = "Button_Delete";
			this.Button_Delete.Size = new System.Drawing.Size(75, 23);
			this.Button_Delete.TabIndex = 7;
			this.Button_Delete.Text = "Delete";
			this.Button_Delete.UseVisualStyleBackColor = true;
			this.Button_Delete.Click += new System.EventHandler(this.Button_Delete_Click);
			// 
			// Button_OK
			// 
			this.Button_OK.Location = new System.Drawing.Point(714, 415);
			this.Button_OK.Name = "Button_OK";
			this.Button_OK.Size = new System.Drawing.Size(75, 23);
			this.Button_OK.TabIndex = 8;
			this.Button_OK.Text = "OK";
			this.Button_OK.UseVisualStyleBackColor = true;
			this.Button_OK.Click += new System.EventHandler(this.Button_OK_Click);
			// 
			// ProjectSelector
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(800, 450);
			this.Controls.Add(this.Button_OK);
			this.Controls.Add(this.Button_Delete);
			this.Controls.Add(this.Button_New);
			this.Controls.Add(this.Button_ProjectPath);
			this.Controls.Add(this.TextBox_ProjectPath);
			this.Controls.Add(this.TextBox_ProjectName);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.ListView_Projects);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximumSize = new System.Drawing.Size(816, 489);
			this.MinimumSize = new System.Drawing.Size(816, 489);
			this.Name = "ProjectSelector";
			this.Text = "File Encrypter - Project Selection";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListView ListView_Projects;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox TextBox_ProjectName;
		private System.Windows.Forms.TextBox TextBox_ProjectPath;
		private System.Windows.Forms.Button Button_ProjectPath;
		private System.Windows.Forms.Button Button_New;
		private System.Windows.Forms.Button Button_Delete;
		private System.Windows.Forms.Button Button_OK;
		private System.Windows.Forms.ColumnHeader columnHeaderProjectName;
		private System.Windows.Forms.ColumnHeader columnHeader1;
	}
}

