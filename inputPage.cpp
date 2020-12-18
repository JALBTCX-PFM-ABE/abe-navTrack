
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

#include "inputPage.hpp"
#include "inputPageHelp.hpp"

inputPage::inputPage (QWidget *parent, OPTIONS *op, QTextEdit **infiles): QWizardPage (parent)
{
  options = op;

  setTitle (tr ("Input Data Files"));

  setPixmap (QWizard::WatermarkPixmap, QPixmap(":/icons/navTrackWatermark.png"));


  //Create new vertical layout

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  //Create new horizontal layout within the vertical layout

  QHBoxLayout *browseBox = new QHBoxLayout;
  browseBox->setSpacing (5);


  //Create new GroupBox

  QGroupBox *fileBox = new QGroupBox (tr ("Files"), this);
  QHBoxLayout *fileBoxLayout = new QHBoxLayout;
  fileBox->setLayout (fileBoxLayout);
  fileBoxLayout->setSpacing (10);


  //Create nw QPushButton within Files GroupBox / horizontal layout

  inputBrowse = new QPushButton (tr ("Browse"), this);
  inputBrowse->setToolTip (tr ("Add input files to the list"));
  inputBrowse->setWhatsThis (inputBrowseText);
  connect (inputBrowse, SIGNAL (clicked ()), this, SLOT (slotInputBrowseClicked ()));
  fileBoxLayout->addWidget (inputBrowse);
  browseBox->addWidget (fileBox, 1);


  //Create new GroupBox

  QGroupBox *dirBox = new QGroupBox (tr ("Directories"), this);
  QHBoxLayout *dirBoxLayout = new QHBoxLayout;
  dirBox->setLayout (dirBoxLayout);
  dirBoxLayout->setSpacing (10);


  //Create new GroupBox within Directories GroupBox / horizontal layout

  QGroupBox *maskBox = new QGroupBox (tr ("Directory file mask"), this);
  QHBoxLayout *maskBoxLayout = new QHBoxLayout;
  maskBox->setLayout (maskBoxLayout);
  maskBoxLayout->setSpacing (10);


  //Create new QComboBox within Directories file mask GroupBox / horizontal layout

  fileMask = new QComboBox (this);
  fileMask->setToolTip (tr ("Set the file mask for the directory Browse button"));
  fileMask->setWhatsThis (fileMaskText);
  fileMask->setEditable (true);
  fileMask->addItem ("GSF (*.d[0-9][0-9] *.gsf)");
  fileMask->addItem ("POS (*.pos *.POS)");
  fileMask->addItem ("SBET (*.out *.OUT)");
  maskBoxLayout->addWidget (fileMask);

  if (options->inputFilter == "POS (*.pos *.POS)")
    {
      fileMask->setCurrentIndex (1);
    }
  if (options->inputFilter == "SBET (*.out *.OUT)")
    {
      fileMask->setCurrentIndex (2);
    }
  else
    {
      fileMask->setCurrentIndex (0);
    }

  connect (fileMask, SIGNAL (currentIndexChanged (const QString &)), this, SLOT (slotFileMaskTextChanged (const QString &)));


  //Add Directories file mask to page

  dirBoxLayout->addWidget (maskBox);


  //Create new QPushButton within Directories GroupBox / horizontal layout

  dirBrowse = new QPushButton (tr ("Browse"), this);
  dirBrowse->setToolTip (tr ("Add input directory contents to the list"));
  dirBrowse->setWhatsThis (dirBrowseText);
  connect (dirBrowse, SIGNAL (clicked ()), this, SLOT (slotDirBrowseClicked ()));
  dirBoxLayout->addWidget (dirBrowse);


  //Add QPushButton to page

  browseBox->addWidget (dirBox, 1);


  //Add all current changes to actual page

  vbox->addLayout (browseBox);


  //Create new GroupBox within main vertical layout

  QGroupBox *inputBox = new QGroupBox (tr ("Input file list"), this);
  QHBoxLayout *inputBoxLayout = new QHBoxLayout;
  inputBox->setLayout (inputBoxLayout);
  inputBoxLayout->setSpacing (10);


  //Create new QTextEdit within Input file list Groupbox / horizontal layout

  inputFiles = new QTextEdit (this);
  inputFiles->setWhatsThis (inputFilesText);
  inputFiles->setLineWrapMode (QTextEdit::NoWrap);
  inputBoxLayout->addWidget (inputFiles);


  //Add Input file list Groupbox to actual page / vertical layout

  vbox->addWidget (inputBox, 10);

  *infiles = inputFiles;
}



//This slot function occurs when the Browse button in the Files GroupBox is clicked
//The function allows the user to specify multiple files for input

void inputPage::slotInputBrowseClicked ()
{
  QFileDialog fd (this, tr ("navTrack Input files"));
  fd.setViewMode (QFileDialog::List);


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (&fd, options->input_dir);

  QStringList filters;
  filters << "GSF (*.d[0-9][0-9] *.gsf)"
          << "POS (*.pos *.POS)"
          << "SBET (*.out *.OUT)";

  fd.setNameFilters (filters);
  fd.setFileMode (QFileDialog::ExistingFiles);
  fd.selectNameFilter (options->inputFilter);

  QStringList files;


  //If user hits [OK] in the browse dialog, we will grab the selected files (if it exists)

  if (fd.exec () == QDialog::Accepted) 
    {
      files = fd.selectedFiles ();

      for (int32_t i = 0 ; i < files.size () ; i++)
        {
          inputFiles->append (files.at (i));
        }
    }

  options->input_dir = fd.directory ().absolutePath ();
  options->inputFilter = fd.selectedNameFilter ();

  if (options->inputFilter == "POS (*.pos *.POS)")
    {
      fileMask->setCurrentIndex (1);
    }
  if (options->inputFilter == "SBET (*.out *.OUT)")
    {
      fileMask->setCurrentIndex (2);
    }
  else
    {
      fileMask->setCurrentIndex (0);
    }
}



//This slot function occurs when the browse button in the Directories GroupBox is clicked
//The function allows the user to specify multiple files for input with a user specified filter

void inputPage::slotDirBrowseClicked ()
{
  QString title = tr ("navTrack Input directories, mask = ") + options->inputFilter;
  QFileDialog *fd = new QFileDialog (this, title);
  fd->setViewMode (QFileDialog::List);


  //  Always add the current working directory and the last used directory to the sidebar URLs in case we're running from the command line.
  //  This function is in the nvutility library.

  setSidebarUrls (fd, options->input_dir);

  QStringList filters;
  filters << "GSF (*.d[0-9][0-9] *.gsf)"
          << "POS (*.pos *.POS)"
          << "SBET (*.out *.OUT)";

  fd->setNameFilters (filters);
  fd->setFileMode (QFileDialog::Directory);
  fd->setNameFilter (options->inputFilter);


  //  Make the directory file dialog support multiple directories.

  QListView *fdListView = fd->findChild<QListView*> ("listView");

  if (fdListView)
    {
      fdListView->setSelectionMode (QAbstractItemView::ExtendedSelection);
    }

  QTreeView *fdTreeView = fd->findChild<QTreeView*> ();

  if (fdTreeView)
    {
      fdTreeView->setSelectionMode (QAbstractItemView::ExtendedSelection);
    }

  QString file;
  QStringList files;


  //If user hits [OK] in the browse dialog, we will grab the selected files (if it exists)

  if (fd->exec () == QDialog::Accepted) 
    {
      //  Save the directory that we were in when we selected a directory.

      options->input_dir = fd->directory ().absolutePath ();

      files = fd->selectedFiles ();

      for (int32_t i = 0 ; i < files.size () ; i++)
        {
          file = files.at (i);

          if (file.isEmpty ()) file = fd->directory ().absolutePath ();

          QDir dirs;
          dirs.cd (file);

          dirs.setFilter (QDir::Dirs | QDir::Readable);


          //  Get all matching files in this directory.

          QDir files;
          files.setFilter (QDir::Files | QDir::Readable);

          QStringList nameFilter;
          if (options->inputFilter == "GSF (*.d[0-9][0-9] *.gsf)")
            {
              nameFilter << "*.d[0-9][0-9]" << "*.gsf";
            }
          else if (options->inputFilter == "POS (*.pos *.POS)")
            {
              nameFilter << "*.pos" << "*.POS";
            }
          else if (options->inputFilter == "SBET (*.out *.OUT)")
            {
              nameFilter << "*.out" << "*.OUT";
            }
          else
            {
              nameFilter << options->inputFilter;
            }

          files.setNameFilters (nameFilter);

          if (files.cd (file))
            {
              QFileInfoList flist = files.entryInfoList ();

              for (int32_t j = 0 ; j < flist.size () ; j++)
                {
                  QString tst = flist.at (j).absoluteFilePath ();

                  inputFiles->append (tst);
                }
            }


          //  Get all directories in this directory.

          QFileInfoList dlist = dirs.entryInfoList ();
          QStringList dirList;

          for (int32_t i = 0 ; i < dlist.size () ; i++)
            {
              if (dlist.at (i).fileName () != "." && dlist.at (i).fileName () != "..") 
                dirList.append (dlist.at (i).absoluteFilePath ());
            }


          //  Get all subordinate directories.

          for (int32_t j = 0 ; j < dirList.size () ; j++)
            {
              QString dirName = dirList.at (j);

              if (dirs.cd (dirName))
                {
                  QFileInfoList nlist = dirs.entryInfoList ();

                  for (int32_t k = 0 ; k < nlist.size () ; k++)
                    {
                      if (nlist.at (k).fileName () != "." && nlist.at (k).fileName () != "..") 
                        dirList.append (nlist.at (k).absoluteFilePath ());
                    }
                }
            }


          //  Get all matching files in all subordinate directories

          for (int32_t j= 0 ; j < dirList.size () ; j++)
            {
              files.setFilter (QDir::Files | QDir::Readable);
              files.setNameFilters (nameFilter);

              QString dirName = dirList.at (j);

              if (files.cd (dirName))
                {
                  QFileInfoList flist = files.entryInfoList ();
      
                  for (int32_t k = 0 ; k < flist.size () ; k++)
                    {
                      QString tst = flist.at (k).absoluteFilePath ();

                      inputFiles->append (tst);
                    }
                }
            }
        }
    }
}



//This function occurs when the filter QComboBox is changed
//The function simply updates the user specified filter to be used when browsing for input files

void inputPage::slotFileMaskTextChanged (const QString &text)
{
  options->inputFilter = text;
}
