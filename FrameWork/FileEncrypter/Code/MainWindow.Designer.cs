
namespace FileEncrypter.Code
{
	partial class MainWindow
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
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
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
			this.TreeView_RawFiles = new System.Windows.Forms.TreeView();
			this.ContextMenu_RawFileNode = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ContextMenuRaw_Encrypt = new System.Windows.Forms.ToolStripMenuItem();
			this.ContextMenuRaw_Delete = new System.Windows.Forms.ToolStripMenuItem();
			this.TreeView_EncryptedFiles = new System.Windows.Forms.TreeView();
			this.Button_BuildSelected = new System.Windows.Forms.Button();
			this.Button_BuildAll = new System.Windows.Forms.Button();
			this.Label_CheckItems = new System.Windows.Forms.Label();
			this.Label_TotalAssets = new System.Windows.Forms.Label();
			this.CheckBox_ForceBuild = new System.Windows.Forms.CheckBox();
			this.Button_ClearSelection = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.Label_EncryptedFiles = new System.Windows.Forms.Label();
			this.ProgressBar_Build = new System.Windows.Forms.ProgressBar();
			this.Label_EncryptingCount = new System.Windows.Forms.Label();
			this.TextBox_EncryptLog = new System.Windows.Forms.RichTextBox();
			this.Button_RefreshRawFiles = new System.Windows.Forms.Button();
			this.ContextMenu_EncryptedFileNode = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.ContextMenuEncrypted_Delete = new System.Windows.Forms.ToolStripMenuItem();
			this.ContextMenuEncrypted_Decrypt = new System.Windows.Forms.ToolStripMenuItem();
			this.Button_RefreshEncryptedFiles = new System.Windows.Forms.Button();
			this.Button_DeleteDecryptedFiles = new System.Windows.Forms.Button();
			this.ContextMenu_RawFileNode.SuspendLayout();
			this.ContextMenu_EncryptedFileNode.SuspendLayout();
			this.SuspendLayout();
			// 
			// TreeView_RawFiles
			// 
			this.TreeView_RawFiles.CheckBoxes = true;
			this.TreeView_RawFiles.ContextMenuStrip = this.ContextMenu_RawFileNode;
			this.TreeView_RawFiles.FullRowSelect = true;
			this.TreeView_RawFiles.Location = new System.Drawing.Point(12, 33);
			this.TreeView_RawFiles.Name = "TreeView_RawFiles";
			this.TreeView_RawFiles.Size = new System.Drawing.Size(577, 568);
			this.TreeView_RawFiles.TabIndex = 0;
			this.TreeView_RawFiles.AfterCheck += new System.Windows.Forms.TreeViewEventHandler(this.TreeView_RawFiles_AfterCheck);
			this.TreeView_RawFiles.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.TreeView_RawFiles_NodeMouseClick);
			// 
			// ContextMenu_RawFileNode
			// 
			this.ContextMenu_RawFileNode.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ContextMenuRaw_Encrypt,
            this.ContextMenuRaw_Delete});
			this.ContextMenu_RawFileNode.Name = "contextMenuStrip1";
			this.ContextMenu_RawFileNode.ShowImageMargin = false;
			this.ContextMenu_RawFileNode.Size = new System.Drawing.Size(90, 48);
			this.ContextMenu_RawFileNode.Text = "Bonjour";
			// 
			// ContextMenuRaw_Encrypt
			// 
			this.ContextMenuRaw_Encrypt.Name = "ContextMenuRaw_Encrypt";
			this.ContextMenuRaw_Encrypt.Size = new System.Drawing.Size(89, 22);
			this.ContextMenuRaw_Encrypt.Text = "Encrypt";
			this.ContextMenuRaw_Encrypt.Click += new System.EventHandler(this.ContextMenuRaw_Encrypt_Click);
			// 
			// ContextMenuRaw_Delete
			// 
			this.ContextMenuRaw_Delete.Name = "ContextMenuRaw_Delete";
			this.ContextMenuRaw_Delete.Size = new System.Drawing.Size(89, 22);
			this.ContextMenuRaw_Delete.Text = "Delete";
			this.ContextMenuRaw_Delete.Click += new System.EventHandler(this.ContextMenuRaw_Delete_Click);
			// 
			// TreeView_EncryptedFiles
			// 
			this.TreeView_EncryptedFiles.Location = new System.Drawing.Point(595, 33);
			this.TreeView_EncryptedFiles.Name = "TreeView_EncryptedFiles";
			this.TreeView_EncryptedFiles.Size = new System.Drawing.Size(577, 568);
			this.TreeView_EncryptedFiles.TabIndex = 1;
			this.TreeView_EncryptedFiles.NodeMouseClick += new System.Windows.Forms.TreeNodeMouseClickEventHandler(this.TreeView_EncryptedFiles_NodeMouseClick);
			// 
			// Button_BuildSelected
			// 
			this.Button_BuildSelected.Location = new System.Drawing.Point(13, 607);
			this.Button_BuildSelected.Name = "Button_BuildSelected";
			this.Button_BuildSelected.Size = new System.Drawing.Size(94, 23);
			this.Button_BuildSelected.TabIndex = 2;
			this.Button_BuildSelected.Text = "Build Selected";
			this.Button_BuildSelected.UseVisualStyleBackColor = true;
			this.Button_BuildSelected.Click += new System.EventHandler(this.Button_BuildSelected_Click);
			// 
			// Button_BuildAll
			// 
			this.Button_BuildAll.Location = new System.Drawing.Point(13, 637);
			this.Button_BuildAll.Name = "Button_BuildAll";
			this.Button_BuildAll.Size = new System.Drawing.Size(94, 23);
			this.Button_BuildAll.TabIndex = 3;
			this.Button_BuildAll.Text = "Build All";
			this.Button_BuildAll.UseVisualStyleBackColor = true;
			this.Button_BuildAll.Click += new System.EventHandler(this.Button_BuildAll_Click);
			// 
			// Label_CheckItems
			// 
			this.Label_CheckItems.AutoSize = true;
			this.Label_CheckItems.Location = new System.Drawing.Point(109, 611);
			this.Label_CheckItems.Name = "Label_CheckItems";
			this.Label_CheckItems.Size = new System.Drawing.Size(60, 15);
			this.Label_CheckItems.TabIndex = 4;
			this.Label_CheckItems.Text = "0 Selected";
			// 
			// Label_TotalAssets
			// 
			this.Label_TotalAssets.AutoSize = true;
			this.Label_TotalAssets.Location = new System.Drawing.Point(109, 641);
			this.Label_TotalAssets.Name = "Label_TotalAssets";
			this.Label_TotalAssets.Size = new System.Drawing.Size(49, 15);
			this.Label_TotalAssets.TabIndex = 5;
			this.Label_TotalAssets.Text = "0 Assets";
			// 
			// CheckBox_ForceBuild
			// 
			this.CheckBox_ForceBuild.AutoSize = true;
			this.CheckBox_ForceBuild.Location = new System.Drawing.Point(92, 666);
			this.CheckBox_ForceBuild.Name = "CheckBox_ForceBuild";
			this.CheckBox_ForceBuild.Size = new System.Drawing.Size(85, 19);
			this.CheckBox_ForceBuild.TabIndex = 6;
			this.CheckBox_ForceBuild.Text = "Force Build";
			this.CheckBox_ForceBuild.UseVisualStyleBackColor = true;
			// 
			// Button_ClearSelection
			// 
			this.Button_ClearSelection.Location = new System.Drawing.Point(494, 607);
			this.Button_ClearSelection.Name = "Button_ClearSelection";
			this.Button_ClearSelection.Size = new System.Drawing.Size(95, 23);
			this.Button_ClearSelection.TabIndex = 8;
			this.Button_ClearSelection.Text = "Clear Selection";
			this.Button_ClearSelection.UseVisualStyleBackColor = true;
			this.Button_ClearSelection.Click += new System.EventHandler(this.Button_ClearSelection_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
			this.label1.Location = new System.Drawing.Point(13, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(80, 21);
			this.label1.TabIndex = 9;
			this.label1.Text = "Raw Files";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Font = new System.Drawing.Font("Segoe UI", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
			this.label2.Location = new System.Drawing.Point(595, 9);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(126, 21);
			this.label2.TabIndex = 10;
			this.label2.Text = "Encrypted Files";
			// 
			// Label_EncryptedFiles
			// 
			this.Label_EncryptedFiles.AutoSize = true;
			this.Label_EncryptedFiles.Location = new System.Drawing.Point(595, 611);
			this.Label_EncryptedFiles.Name = "Label_EncryptedFiles";
			this.Label_EncryptedFiles.RightToLeft = System.Windows.Forms.RightToLeft.No;
			this.Label_EncryptedFiles.Size = new System.Drawing.Size(105, 15);
			this.Label_EncryptedFiles.TabIndex = 11;
			this.Label_EncryptedFiles.Text = "0 Encrypted Assets";
			// 
			// ProgressBar_Build
			// 
			this.ProgressBar_Build.Location = new System.Drawing.Point(13, 690);
			this.ProgressBar_Build.Name = "ProgressBar_Build";
			this.ProgressBar_Build.Size = new System.Drawing.Size(1159, 23);
			this.ProgressBar_Build.TabIndex = 12;
			// 
			// Label_EncryptingCount
			// 
			this.Label_EncryptingCount.AutoSize = true;
			this.Label_EncryptingCount.Location = new System.Drawing.Point(13, 716);
			this.Label_EncryptingCount.Name = "Label_EncryptingCount";
			this.Label_EncryptingCount.Size = new System.Drawing.Size(39, 15);
			this.Label_EncryptingCount.TabIndex = 13;
			this.Label_EncryptingCount.Text = "0 /450";
			this.Label_EncryptingCount.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// TextBox_EncryptLog
			// 
			this.TextBox_EncryptLog.Location = new System.Drawing.Point(12, 734);
			this.TextBox_EncryptLog.Name = "TextBox_EncryptLog";
			this.TextBox_EncryptLog.ReadOnly = true;
			this.TextBox_EncryptLog.Size = new System.Drawing.Size(1160, 415);
			this.TextBox_EncryptLog.TabIndex = 14;
			this.TextBox_EncryptLog.Text = "";
			// 
			// Button_RefreshRawFiles
			// 
			this.Button_RefreshRawFiles.Font = new System.Drawing.Font("Roboto Mono", 50F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
			this.Button_RefreshRawFiles.Image = ((System.Drawing.Image)(resources.GetObject("Button_RefreshRawFiles.Image")));
			this.Button_RefreshRawFiles.Location = new System.Drawing.Point(563, 35);
			this.Button_RefreshRawFiles.Name = "Button_RefreshRawFiles";
			this.Button_RefreshRawFiles.Size = new System.Drawing.Size(24, 24);
			this.Button_RefreshRawFiles.TabIndex = 15;
			this.Button_RefreshRawFiles.UseVisualStyleBackColor = true;
			this.Button_RefreshRawFiles.Click += new System.EventHandler(this.Button_RefreshRawFiles_Click);
			// 
			// ContextMenu_EncryptedFileNode
			// 
			this.ContextMenu_EncryptedFileNode.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ContextMenuEncrypted_Delete,
            this.ContextMenuEncrypted_Decrypt});
			this.ContextMenu_EncryptedFileNode.Name = "contextMenuStrip1";
			this.ContextMenu_EncryptedFileNode.Size = new System.Drawing.Size(116, 48);
			// 
			// ContextMenuEncrypted_Delete
			// 
			this.ContextMenuEncrypted_Delete.Name = "ContextMenuEncrypted_Delete";
			this.ContextMenuEncrypted_Delete.Size = new System.Drawing.Size(115, 22);
			this.ContextMenuEncrypted_Delete.Text = "Delete";
			this.ContextMenuEncrypted_Delete.Click += new System.EventHandler(this.ContextMenuEncrypted_Delete_Click);
			// 
			// ContextMenuEncrypted_Decrypt
			// 
			this.ContextMenuEncrypted_Decrypt.Name = "ContextMenuEncrypted_Decrypt";
			this.ContextMenuEncrypted_Decrypt.Size = new System.Drawing.Size(115, 22);
			this.ContextMenuEncrypted_Decrypt.Text = "Decrypt";
			this.ContextMenuEncrypted_Decrypt.Click += new System.EventHandler(this.ContextMenuEncrypted_Decrypt_Click);
			// 
			// Button_RefreshEncryptedFiles
			// 
			this.Button_RefreshEncryptedFiles.Font = new System.Drawing.Font("Roboto Mono", 50F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point);
			this.Button_RefreshEncryptedFiles.Image = ((System.Drawing.Image)(resources.GetObject("Button_RefreshEncryptedFiles.Image")));
			this.Button_RefreshEncryptedFiles.Location = new System.Drawing.Point(1146, 35);
			this.Button_RefreshEncryptedFiles.Name = "Button_RefreshEncryptedFiles";
			this.Button_RefreshEncryptedFiles.Size = new System.Drawing.Size(24, 24);
			this.Button_RefreshEncryptedFiles.TabIndex = 16;
			this.Button_RefreshEncryptedFiles.UseVisualStyleBackColor = true;
			// 
			// Button_DeleteDecryptedFiles
			// 
			this.Button_DeleteDecryptedFiles.Location = new System.Drawing.Point(1041, 607);
			this.Button_DeleteDecryptedFiles.Name = "Button_DeleteDecryptedFiles";
			this.Button_DeleteDecryptedFiles.Size = new System.Drawing.Size(131, 23);
			this.Button_DeleteDecryptedFiles.TabIndex = 17;
			this.Button_DeleteDecryptedFiles.Text = "Delete Decrypted Files";
			this.Button_DeleteDecryptedFiles.UseVisualStyleBackColor = true;
			// 
			// MainWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1184, 1161);
			this.Controls.Add(this.Button_DeleteDecryptedFiles);
			this.Controls.Add(this.Button_RefreshEncryptedFiles);
			this.Controls.Add(this.Button_RefreshRawFiles);
			this.Controls.Add(this.TextBox_EncryptLog);
			this.Controls.Add(this.Label_EncryptingCount);
			this.Controls.Add(this.ProgressBar_Build);
			this.Controls.Add(this.Label_EncryptedFiles);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.Button_ClearSelection);
			this.Controls.Add(this.CheckBox_ForceBuild);
			this.Controls.Add(this.Label_TotalAssets);
			this.Controls.Add(this.Label_CheckItems);
			this.Controls.Add(this.Button_BuildAll);
			this.Controls.Add(this.Button_BuildSelected);
			this.Controls.Add(this.TreeView_EncryptedFiles);
			this.Controls.Add(this.TreeView_RawFiles);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "MainWindow";
			this.Text = "File Encrypter";
			this.ContextMenu_RawFileNode.ResumeLayout(false);
			this.ContextMenu_EncryptedFileNode.ResumeLayout(false);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TreeView TreeView_RawFiles;
		private System.Windows.Forms.TreeView TreeView_EncryptedFiles;
		private System.Windows.Forms.Button Button_BuildSelected;
		private System.Windows.Forms.Button Button_BuildAll;
		private System.Windows.Forms.Label Label_CheckItems;
		private System.Windows.Forms.Label Label_TotalAssets;
		private System.Windows.Forms.CheckBox CheckBox_ForceBuild;
		private System.Windows.Forms.Button Button_ClearSelection;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label Label_EncryptedFiles;
		private System.Windows.Forms.ProgressBar ProgressBar_Build;
		private System.Windows.Forms.Label Label_EncryptingCount;
		private System.Windows.Forms.RichTextBox TextBox_EncryptLog;
		private System.Windows.Forms.Button Button_RefreshRawFiles;
		private System.Windows.Forms.ContextMenuStrip ContextMenu_RawFileNode;
		private System.Windows.Forms.ToolStripMenuItem ContextMenuRaw_Delete;
		private System.Windows.Forms.ContextMenuStrip ContextMenu_EncryptedFileNode;
		private System.Windows.Forms.ToolStripMenuItem ContextMenuEncrypted_Delete;
		private System.Windows.Forms.ToolStripMenuItem ContextMenuRaw_Encrypt;
		private System.Windows.Forms.ToolStripMenuItem ContextMenuEncrypted_Decrypt;
		private System.Windows.Forms.Button Button_RefreshEncryptedFiles;
		private System.Windows.Forms.Button Button_DeleteDecryptedFiles;
	}
}