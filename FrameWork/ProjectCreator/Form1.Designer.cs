namespace ProjectCreator
{
	partial class Form1
	{
		/// <summary>
		///  Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		///  Clean up any resources being used.
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
		///  Required method for Designer support - do not modify
		///  the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
			Btn_LocateSln = new Button();
			label1 = new Label();
			TxtBx_LastSolutionLocation = new TextBox();
			TxtBx_ProjectName = new TextBox();
			TxtBx_Logs = new RichTextBox();
			Combo_ProjectTypes = new ComboBox();
			Btn_CreateProject = new Button();
			treeView1 = new TreeView();
			label2 = new Label();
			label3 = new Label();
			RdBtn_Exe = new RadioButton();
			RdBtn_Dll = new RadioButton();
			label4 = new Label();
			label5 = new Label();
			comboBox1 = new ComboBox();
			TxtBx_SourcesDirectoryName = new TextBox();
			SuspendLayout();
			// 
			// Btn_LocateSln
			// 
			Btn_LocateSln.Location = new Point(12, 12);
			Btn_LocateSln.Name = "Btn_LocateSln";
			Btn_LocateSln.Size = new Size(112, 23);
			Btn_LocateSln.TabIndex = 0;
			Btn_LocateSln.Text = "Locate solution";
			Btn_LocateSln.UseVisualStyleBackColor = true;
			Btn_LocateSln.Click += Btn_LocateSln_Click;
			// 
			// label1
			// 
			label1.AutoSize = true;
			label1.Location = new Point(206, 17);
			label1.Name = "label1";
			label1.Size = new Size(101, 15);
			label1.TabIndex = 1;
			label1.Text = "Last solution path";
			// 
			// TxtBx_LastSolutionLocation
			// 
			TxtBx_LastSolutionLocation.Font = new Font("Segoe UI", 9F, FontStyle.Regular, GraphicsUnit.Point);
			TxtBx_LastSolutionLocation.ForeColor = Color.Red;
			TxtBx_LastSolutionLocation.Location = new Point(313, 13);
			TxtBx_LastSolutionLocation.Name = "TxtBx_LastSolutionLocation";
			TxtBx_LastSolutionLocation.ReadOnly = true;
			TxtBx_LastSolutionLocation.Size = new Size(509, 23);
			TxtBx_LastSolutionLocation.TabIndex = 2;
			// 
			// TxtBx_ProjectName
			// 
			TxtBx_ProjectName.Location = new Point(216, 55);
			TxtBx_ProjectName.Name = "TxtBx_ProjectName";
			TxtBx_ProjectName.Size = new Size(112, 23);
			TxtBx_ProjectName.TabIndex = 3;
			TxtBx_ProjectName.Text = "Name";
			// 
			// TxtBx_Logs
			// 
			TxtBx_Logs.Location = new Point(334, 55);
			TxtBx_Logs.Name = "TxtBx_Logs";
			TxtBx_Logs.Size = new Size(488, 344);
			TxtBx_Logs.TabIndex = 4;
			TxtBx_Logs.Text = "";
			// 
			// Combo_ProjectTypes
			// 
			Combo_ProjectTypes.DropDownStyle = ComboBoxStyle.DropDownList;
			Combo_ProjectTypes.FormattingEnabled = true;
			Combo_ProjectTypes.Location = new Point(87, 55);
			Combo_ProjectTypes.Name = "Combo_ProjectTypes";
			Combo_ProjectTypes.Size = new Size(121, 23);
			Combo_ProjectTypes.TabIndex = 5;
			// 
			// Btn_CreateProject
			// 
			Btn_CreateProject.Location = new Point(28, 379);
			Btn_CreateProject.Name = "Btn_CreateProject";
			Btn_CreateProject.Size = new Size(279, 23);
			Btn_CreateProject.TabIndex = 6;
			Btn_CreateProject.Text = "Create";
			Btn_CreateProject.UseVisualStyleBackColor = true;
			Btn_CreateProject.Click += Btn_CreateProject_Click;
			// 
			// treeView1
			// 
			treeView1.Location = new Point(11, 196);
			treeView1.Name = "treeView1";
			treeView1.Size = new Size(317, 177);
			treeView1.TabIndex = 7;
			// 
			// label2
			// 
			label2.AutoSize = true;
			label2.Location = new Point(12, 178);
			label2.Name = "label2";
			label2.Size = new Size(92, 15);
			label2.TabIndex = 8;
			label2.Text = "Needed libraries";
			// 
			// label3
			// 
			label3.AutoSize = true;
			label3.Location = new Point(11, 58);
			label3.Name = "label3";
			label3.Size = new Size(55, 15);
			label3.TabIndex = 9;
			label3.Text = "Category";
			// 
			// RdBtn_Exe
			// 
			RdBtn_Exe.AutoSize = true;
			RdBtn_Exe.Checked = true;
			RdBtn_Exe.Location = new Point(120, 114);
			RdBtn_Exe.Name = "RdBtn_Exe";
			RdBtn_Exe.Size = new Size(43, 19);
			RdBtn_Exe.TabIndex = 10;
			RdBtn_Exe.TabStop = true;
			RdBtn_Exe.Text = "exe";
			RdBtn_Exe.UseVisualStyleBackColor = true;
			// 
			// RdBtn_Dll
			// 
			RdBtn_Dll.AutoSize = true;
			RdBtn_Dll.Location = new Point(169, 114);
			RdBtn_Dll.Name = "RdBtn_Dll";
			RdBtn_Dll.Size = new Size(38, 19);
			RdBtn_Dll.TabIndex = 11;
			RdBtn_Dll.Text = "dll";
			RdBtn_Dll.UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			label4.AutoSize = true;
			label4.Location = new Point(12, 116);
			label4.Name = "label4";
			label4.Size = new Size(103, 15);
			label4.TabIndex = 12;
			label4.Text = "Produced file type";
			// 
			// label5
			// 
			label5.AutoSize = true;
			label5.Location = new Point(11, 87);
			label5.Name = "label5";
			label5.Size = new Size(31, 15);
			label5.TabIndex = 13;
			label5.Text = "Type";
			// 
			// comboBox1
			// 
			comboBox1.DropDownStyle = ComboBoxStyle.DropDownList;
			comboBox1.FormattingEnabled = true;
			comboBox1.Items.AddRange(new object[] { "Application", "Game" });
			comboBox1.Location = new Point(87, 86);
			comboBox1.Name = "comboBox1";
			comboBox1.Size = new Size(121, 23);
			comboBox1.TabIndex = 14;
			// 
			// TxtBx_SourcesDirectoryName
			// 
			TxtBx_SourcesDirectoryName.Location = new Point(216, 86);
			TxtBx_SourcesDirectoryName.Name = "TxtBx_SourcesDirectoryName";
			TxtBx_SourcesDirectoryName.Size = new Size(112, 23);
			TxtBx_SourcesDirectoryName.TabIndex = 15;
			TxtBx_SourcesDirectoryName.Text = "Sources";
			// 
			// Form1
			// 
			AutoScaleDimensions = new SizeF(7F, 15F);
			AutoScaleMode = AutoScaleMode.Font;
			ClientSize = new Size(834, 411);
			Controls.Add(TxtBx_SourcesDirectoryName);
			Controls.Add(comboBox1);
			Controls.Add(label5);
			Controls.Add(label4);
			Controls.Add(RdBtn_Dll);
			Controls.Add(RdBtn_Exe);
			Controls.Add(label3);
			Controls.Add(label2);
			Controls.Add(treeView1);
			Controls.Add(Btn_CreateProject);
			Controls.Add(Combo_ProjectTypes);
			Controls.Add(TxtBx_Logs);
			Controls.Add(TxtBx_ProjectName);
			Controls.Add(TxtBx_LastSolutionLocation);
			Controls.Add(label1);
			Controls.Add(Btn_LocateSln);
			FormBorderStyle = FormBorderStyle.FixedSingle;
			Icon = (Icon)resources.GetObject("$this.Icon");
			Name = "Form1";
			Text = "FaZoN - Project Creator";
			ResumeLayout(false);
			PerformLayout();
		}

		#endregion

		private Button Btn_LocateSln;
		private Label label1;
		private TextBox TxtBx_LastSolutionLocation;
		private RichTextBox TxtBx_Logs;
		private Button Btn_CreateProject;
		private TreeView treeView1;
		private Label label2;
		private Label label3;
		private RadioButton RdBtn_Exe;
		private Label label4;
		private Label label5;
		public TextBox TxtBx_ProjectName;
		public ComboBox Combo_ProjectTypes;
		private RadioButton RdBtn_Dll;
		public TextBox TxtBx_SourcesDirectoryName;
		public ComboBox comboBox1;
	}
}