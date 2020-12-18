
/*********************************************************************************************

    This is public domain software that was developed by or for the U.S. Naval Oceanographic
    Office and/or the U.S. Army Corps of Engineers.

    This is a work of the U.S. Government. In accordance with 17 USC 105, copyright protection
    is not available for any work of the U.S. Government.

    Neither the United States Government, nor any employees of the United States Government,
    nor the author, makes any warranty, express or implied, without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, or assumes any liability or
    responsibility for the accuracy, completeness, or usefulness of any information,
    apparatus, product, or process disclosed, or represents that its use would not infringe
    privately-owned rights. Reference herein to any specific commercial products, process,
    or service by trade name, trademark, manufacturer, or otherwise, does not necessarily
    constitute or imply its endorsement, recommendation, or favoring by the United States
    Government. The views and opinions of authors expressed herein do not necessarily state
    or reflect those of the United States Government, and shall not be used for advertising
    or product endorsement purposes.

*********************************************************************************************/

#include "startPage.hpp"
#include "startPageHelp.hpp"

startPage::startPage (QWidget *parent, OPTIONS *op): QWizardPage (parent)
{
  options = op;

  setPixmap (QWizard::WatermarkPixmap, QPixmap(":/icons/navTrackWatermark.png"));

  setTitle (tr ("Introduction"));

  setWhatsThis (tr ("See, it really works!"));


  //This is the old label
  //QLabel *label = new QLabel (tr ("navTrack is a tool for building an ASCII decimated navTrack file from GSF or POS/SBET "
  //                                "data.  The resulting file is viewable in areaCheck or as an overlay in "
  //                                "pfmView.  Help is available by clicking on the Help button and then clicking on the item "
  //                                "for which you want help.  Click <b>Next</b> to continue or <b>Cancel</b> to exit."));


  QLabel *label = new QLabel (tr ("navTrack is a tool both for building an ASCII decimated navTrack file from GSF or POS/SBET "
                                  "data, and for creating a decimated 4-word file similiar to the output "
                                  "generated from the gsf24word program except that it may also use POS/SBET "
                                  "data as input.  The ASCII decimated navTrack file is viewable in areaCheck or as an overlay "
                                  "in pfmView.  The decimated 4-word file may be used with any program that takes a 4-word file "
                                  "as input.  Help is available by clicking on the Help button and then clicking on the item "
                                  "for which you want help.  Click <b>Next</b> to continue or <b>Cancel</b> to exit."));

  label->setWordWrap(true);
  label->setWhatsThis (labelText);

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->addWidget (label);
  vbox->addStretch (10);

  trackCheckBox = new QCheckBox(tr ("Click to create a .tr&k file"), this);
  trackCheckBox->setToolTip(tr ("Select this option to create a .trk file"));
  trackCheckBox->setWhatsThis (trackCheckBoxText);
  vbox->addWidget(trackCheckBox);

  oBox = new QGroupBox (tr ("navTrack output file"), this);
  QHBoxLayout *oBoxLayout = new QHBoxLayout;
  oBox->setLayout (oBoxLayout);
  oBoxLayout->setSpacing (10);

  outputFile = new QLineEdit (this);
  outputFile->setWhatsThis (outputFileText);
  oBoxLayout->addWidget (outputFile, 10);

  QPushButton *output_file_browse = new QPushButton (tr ("Browse..."), this);
  output_file_browse->setWhatsThis (output_fileBrowseText);
  connect (output_file_browse, SIGNAL (clicked ()), this, SLOT (slotOutputFileBrowse ()));
  oBoxLayout->addWidget (output_file_browse, 1);

  vbox->addWidget (oBox);

  registerField ("outputFile", outputFile);  

  oBox->setEnabled(false);

  connect(trackCheckBox, SIGNAL(clicked (bool)), this, SLOT (slotCustomCheckBoxChecked (bool)));
}



//This slot function occurs when the trackCheckBox is clicked
//The function simply enables or disables the ability to create a .trk file

void startPage::slotCustomCheckBoxChecked(bool mode)
{
  if(mode)
    {
      oBox->setEnabled(true);
      options->isMakeTRK = 1;
      outputFile->setText("defaultTRKFileOutput.trk");
    }
  else
    {
      oBox->setEnabled(false);
      options->isMakeTRK = 0;
      outputFile->setText("");
    }
}



//This slot function occurs when the Browse button is clicked
//The function allows the user to browse for a particular .trk output file already in existence

void startPage::slotOutputFileBrowse ()
{
  QFileDialog fd (this, tr ("navTrack Output file"));
  fd.setViewMode (QFileDialog::List);


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (&fd, options->output_dir);

  QStringList filters;
  filters << tr ("navTrack file (*.trk)");

  fd.setNameFilters (filters);
  fd.setFileMode (QFileDialog::AnyFile);

  QStringList files;


  //If user hits [OK] in the browse dialog, we will grab the selected files (if it exists)

  if (fd.exec () == QDialog::Accepted) 
    {
      files = fd.selectedFiles ();

      QString file = files.at (0);

      if (!file.isEmpty ())
        {
          outputFile->setText (file);
        }
    }

  options->output_dir = fd.directory ().absolutePath ();
}
