
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

#include "navOptionsPage.hpp"
#include "navOptionsPageHelp.hpp"
#include <string.h>
#include <qstringlist.h>

navOptionsPage::navOptionsPage (QWidget *parent, OPTIONS *ops):
  QWizardPage (parent)
{
  options = ops;
  FILE *shipInFile;

  setTitle (tr ("nav File Options"));

  setPixmap (QWizard::WatermarkPixmap, QPixmap(":/icons/navTrackWatermark.png"));

  QVBoxLayout *mainLayout = new QVBoxLayout (this);
  mainLayout->setMargin (5);
  mainLayout->setSpacing (5);

  navCheckBox = new QCheckBox (tr ("Click to create .na&v file"), this);  
  navCheckBox->setToolTip (tr ("Select this option to create a 4word .nav file"));
  navCheckBox->setWhatsThis(QCheckBoxText);
  mainLayout->addWidget(navCheckBox);

  headBox = new QGroupBox (tr ("Header Options"), this);
  QHBoxLayout *headLayout = new QHBoxLayout;
  headBox->setLayout (headLayout);
  QVBoxLayout *headLeftLayout = new QVBoxLayout;
  QVBoxLayout *headRightLayout = new QVBoxLayout;
  headLayout->addLayout (headLeftLayout);
  headLayout->addLayout (headRightLayout);

  QGroupBox *shipBox = new QGroupBox (tr ("Ship ID"), this);
  QHBoxLayout *shipBoxLayout = new QHBoxLayout;
  shipBox->setLayout (shipBoxLayout);
  shipBoxLayout->setSpacing (10);

  shipID = new QComboBox(this);

  lock_noShipIDFile = false;
   
  char shipIDFilename[80] = "/net/redhat13/data1/BHPP/local/shiptable.txt";

  /*char shipIDFilename[80] = "/data1/byrdk/myShips.txt";*/
  char readFileLine[100];
  char *tokenizer;
        
  QString insertShipID;

  if ((shipInFile = fopen (shipIDFilename, "r")) == NULL)
    {
      strcpy(shipIDFilename, "/net/fatboy/data1/BHPP/local/shiptable.txt");

      if((shipInFile = fopen (shipIDFilename, "r")) == NULL)
        {
          QMessageBox::critical (this, tr ("navTrack"), tr ("Unable to open shiptable.txt!  No .nav files "
                                                            "may be created, only .trk files."));
          lock_noShipIDFile = true;
          navCheckBox->setEnabled(false);
        }
      else
        {
          while(fgets(readFileLine, sizeof(readFileLine), shipInFile) != NULL)
            {
              if(readFileLine[0] != '#')
                {
                  tokenizer = strtok(readFileLine, ",");
                  insertShipID = QString(tokenizer);        

                  insertShipID.append(" - ");

                  tokenizer = strtok(NULL, ",");
                  insertShipID.append(tokenizer);

                  shipID->addItem(insertShipID);
                }
            }
        }
    }
  else
    {
      while(fgets(readFileLine, sizeof(readFileLine), shipInFile) != NULL)
        {
          if(readFileLine[0] != '#')
            {
              tokenizer = strtok(readFileLine, ",");
              insertShipID = QString(tokenizer);

              insertShipID.append(" - ");

              tokenizer = strtok(NULL, ",");
              insertShipID.append(tokenizer);

              shipID->addItem(insertShipID);
            }
        }
    }


  shipID->setToolTip(tr("Select survey platform"));
  shipID->setWhatsThis(shipIDText);
  shipBoxLayout->addWidget(shipID);
  headLeftLayout->addWidget(shipBox);
  
  QGroupBox *processBox = new QGroupBox(tr("Process"), this);
  QHBoxLayout *processBoxLayout = new QHBoxLayout;
  processBox->setLayout(processBoxLayout);
  processBoxLayout->setSpacing(10);

  navProcess = new QComboBox(this);
  navProcess->addItems(processList);
  navProcess->setToolTip(tr("Select processing type"));
  navProcess->setWhatsThis(navProcessText);
  processBoxLayout->addWidget(navProcess);
  headRightLayout->addWidget(processBox);

  QGroupBox *accessBox = new QGroupBox(tr("Access"), this);
  QHBoxLayout *accessBoxLayout = new QHBoxLayout;
  accessBox->setLayout(accessBoxLayout);
  accessBoxLayout->setSpacing(10);

  navAccess = new QComboBox(this);
  navAccess->addItems(accessList);
  navAccess->setToolTip(tr("Choose classification for nav data"));
  navAccess->setWhatsThis(navAccessText);
  accessBoxLayout->addWidget(navAccess);
  headLeftLayout->addWidget(accessBox);

  QGroupBox *datumBox = new QGroupBox(tr("Datum"), this);
  QHBoxLayout *datumBoxLayout = new QHBoxLayout;
  datumBox->setLayout(datumBoxLayout);
  datumBoxLayout->setSpacing(10);

  navDatum = new QComboBox(this);
  navDatum->addItems(datumList);
  navDatum->setToolTip(tr("Choose datum for nav data"));
  navDatum->setWhatsThis(navDatumText);
  datumBoxLayout->addWidget(navDatum);
  headRightLayout->addWidget(datumBox);

  QGroupBox *survOpBox = new QGroupBox(tr("Survey Op (Format: YYNN)"), this);
  QHBoxLayout *survOpBoxLayout = new QHBoxLayout;
  survOpBox->setLayout(survOpBoxLayout);
  survOpBoxLayout->setSpacing(10);  

  navSurvOp = new QLineEdit(this);
  navSurvOp->setMaxLength(16);
  navSurvOp->setToolTip(tr("Enter survey op (Format:  YYNN)"));
  navSurvOp->setWhatsThis(navSurvOpText);
  survOpBoxLayout->addWidget(navSurvOp, 10);
  headLeftLayout->addWidget(survOpBox); 

  mainLayout->addWidget(headBox);

  outputFileBox = new QGroupBox(tr("Nav Output File"), this);
  QHBoxLayout *outputFileBoxLayout = new QHBoxLayout;
  outputFileBox->setLayout(outputFileBoxLayout);

  outputFile = new QLineEdit(this);
  outputFile->setWhatsThis(navOutputFileText);
  outputFileBoxLayout->addWidget(outputFile, 10);

  output_file_browse = new QPushButton(tr("Browse..."), this);
  output_file_browse->setWhatsThis(output_file_browseText);
  connect(output_file_browse, SIGNAL (clicked()), this, SLOT (slotOutputFileBrowse()));
  outputFileBoxLayout->addWidget(output_file_browse, 1);

  mainLayout->addWidget(outputFileBox);

  headBox->setEnabled(false);
  outputFileBox->setEnabled(false);

  registerField ("navOutputFile", outputFile);
  registerField ("shipID", shipID);
  registerField ("survOp", navSurvOp);
  registerField ("navProcess", navProcess);
  registerField ("navAccess", navAccess);
  registerField ("navDatum", navDatum);

  connect (navCheckBox, SIGNAL (clicked (bool)), this, SLOT (slotCustomCheckBoxChecked (bool)));

  connect (shipID, SIGNAL (currentIndexChanged(QString)), this, SLOT (slotCustomQComboChange (QString)));
}



void navOptionsPage::slotCustomQComboChange(QString text)
{
  myByteArray = text.toLatin1();
  charConvert = myByteArray.data();
  token = strtok(charConvert, ",");

  options->ship_identity = atoi(token);
}



void navOptionsPage::slotCustomCheckBoxChecked(bool mode)
{
  if(mode && !lock_noShipIDFile)
    {
      headBox->setEnabled(true);
      outputFileBox->setEnabled(true);
      options->isMakeNAV = 1;
      outputFile->setText("defaultNAVFileOutput.nav");
      navSurvOp->setPlaceholderText("MANDATORY FIELD");
    }
  else
    {
      headBox->setEnabled(false);
      outputFileBox->setEnabled(false);
      options->isMakeNAV = 0;
      outputFile->setText("");
      navSurvOp->setPlaceholderText("");
    }
}



void navOptionsPage::slotOutputFileBrowse ()
{
  QFileDialog fd (this, tr ("nav Output file"));
  fd.setViewMode (QFileDialog::List);


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (&fd, options->navOutput_dir);


  QStringList filters;
  filters << tr ("4word nav file (*.nav)");


  fd.setNameFilters (filters);
  fd.setFileMode (QFileDialog::AnyFile);


  QStringList files;


  if (fd.exec () == QDialog::Accepted) 
    {
      files = fd.selectedFiles ();

      QString file = files.at (0);

      if (!file.isEmpty ())
        {
          outputFile->setText (file);
        }
    }

  options->navOutput_dir = fd.directory ().absolutePath ();
}
