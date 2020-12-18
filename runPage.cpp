
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

#include "runPage.hpp"
#include "runPageHelp.hpp"

runPage::runPage (QWidget *parent, RUN_PROGRESS *prog, QListWidget **tList): QWizardPage (parent)
{
  progress = prog;

  setTitle (tr ("Build navTrack file"));

  setPixmap (QWizard::WatermarkPixmap, QPixmap(":/icons/navTrackWatermark.png"));


  //Last page of wizard

  setFinalPage (true);


  //Create vertical layout of page

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  //Create new GroupBox

  progress->fbox = new QGroupBox (tr ("Input file processing progress"), this);
  QVBoxLayout *fboxLayout = new QVBoxLayout;
  progress->fbox->setLayout (fboxLayout);
  fboxLayout->setSpacing (10);


  //Create new QProgressBar within above GroupBox / vertical layout

  progress->fbar = new QProgressBar (this);
  progress->fbar->setRange (0, 100);
  progress->fbar->setWhatsThis (tr ("Progress of input file loading"));
  fboxLayout->addWidget (progress->fbar);


  //Add recent changes to page

  vbox->addWidget (progress->fbox);


  //Create new GroupBox

  QGroupBox *lbox = new QGroupBox (tr ("Process status"), this);
  QVBoxLayout *lboxLayout = new QVBoxLayout;
  lbox->setLayout (lboxLayout);
  lboxLayout->setSpacing (10);


  //Create new QListWidget within Process Status GroupBox / vertical layout

  trackList = new QListWidget (this);
  trackList->setAlternatingRowColors (true);
  lboxLayout->addWidget (trackList);


  //Add everything up to now to page

  vbox->addWidget (lbox);

  *tList = trackList;


  //  Serious cheating here ;-)  I want the finish button to be disabled when you first get to this page
  //  so I set the last progress bar to be a "mandatory" field.  I couldn't disable the damn button in
  //  initializePage in the parent for some unknown reason.

  registerField ("progress_fbar*", progress->fbar, "value");
}
