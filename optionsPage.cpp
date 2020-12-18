
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

#include "optionsPage.hpp"
#include "optionsPageHelp.hpp"

optionsPage::optionsPage (QWidget *parent, OPTIONS *op): QWizardPage (parent)
{
  options = op;

  setTitle (tr ("navTrack Options"));

  setPixmap (QWizard::WatermarkPixmap, QPixmap(":/icons/navTrackWatermark.png"));


  //Create new verticle layout

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setMargin (5);
  vbox->setSpacing (5);


  //Create new GroupBox

  QGroupBox *lbox = new QGroupBox (tr ("Limits"), this);
  QHBoxLayout *lboxLayout = new QHBoxLayout;
  lbox->setLayout (lboxLayout);


  //Create new GroupBox with horizontal layout

  QGroupBox *gBox = new QGroupBox (tr ("GSF"), this);
  QHBoxLayout *gBoxLayout = new QHBoxLayout;
  gBox->setLayout (gBoxLayout);
  gBoxLayout->setSpacing (10);


  //Create new GroupBox within GSF GroupBox / horizontal layout

  QGroupBox *ghBox = new QGroupBox (tr ("Heading"), this);
  QHBoxLayout *ghBoxLayout = new QHBoxLayout;
  ghBox->setLayout (ghBoxLayout);
  ghBoxLayout->setSpacing (10);


  //Create new QDoubleSpinBox and add to page

  headingGSF = new QDoubleSpinBox (this);
  headingGSF->setDecimals (1);
  headingGSF->setRange (1.0, 50.0);
  headingGSF->setSingleStep (1.0);
  headingGSF->setValue (options->heading[NAVTRACK_GSF]);
  headingGSF->setWrapping (true);
  headingGSF->setToolTip (tr ("Set the heading limit for GSF files (degrees)"));
  headingGSF->setWhatsThis (headingGSFText);
  ghBoxLayout->addWidget (headingGSF);
  gBoxLayout->addWidget (ghBox);


  //Create new GroupBox within GSF GroupBox / horizontal layout

  QGroupBox *gsBox = new QGroupBox (tr ("Speed"), this);
  QHBoxLayout *gsBoxLayout = new QHBoxLayout;
  gsBox->setLayout (gsBoxLayout);
  gsBoxLayout->setSpacing (10);


  //Create new QDoubleSpinBox and add to page

  speedGSF = new QDoubleSpinBox (this);
  speedGSF->setDecimals (1);
  speedGSF->setRange (1.0, 50.0);
  speedGSF->setSingleStep (1.0);
  speedGSF->setValue (options->speed[NAVTRACK_GSF]);
  speedGSF->setWrapping (true);
  speedGSF->setToolTip (tr ("Set the speed limit for GSF files (knots)"));
  speedGSF->setWhatsThis (speedGSFText);
  gsBoxLayout->addWidget (speedGSF);
  gBoxLayout->addWidget (gsBox);


  //Add recent additions to Limits GroupBox / vertical layout

  lboxLayout->addWidget (gBox);


  //Create new GroupBox with horizontal layout

  QGroupBox *pBox = new QGroupBox (tr ("POS/SBET"), this);
  QHBoxLayout *pBoxLayout = new QHBoxLayout;
  pBox->setLayout (pBoxLayout);
  pBoxLayout->setSpacing (10);


  //Create new GroupBox within POS/SBET GroupBox / horizontal layout

  QGroupBox *phBox = new QGroupBox (tr ("Heading"), this);
  QHBoxLayout *phBoxLayout = new QHBoxLayout;
  phBox->setLayout (phBoxLayout);
  phBoxLayout->setSpacing (10);


  //Create new QDoubleSpinBox and add to page

  headingPOS = new QDoubleSpinBox (this);
  headingPOS->setDecimals (1);
  headingPOS->setRange (1.0, 50.0);
  headingPOS->setSingleStep (1.0);
  headingPOS->setValue (options->heading[NAVTRACK_POS]);
  headingPOS->setWrapping (true);
  headingPOS->setToolTip (tr ("Set the heading limit for POS/SBET files (degrees)"));
  headingPOS->setWhatsThis (headingPOSText);
  phBoxLayout->addWidget (headingPOS);
  pBoxLayout->addWidget (phBox);


  //Create new GroupBox within POS/SBET GroupBox / horizontal layout

  QGroupBox *psBox = new QGroupBox (tr ("Speed"), this);
  QHBoxLayout *psBoxLayout = new QHBoxLayout;
  psBox->setLayout (psBoxLayout);
  psBoxLayout->setSpacing (10);


  //Create new QDoubleSpinBox and add to page

  speedPOS = new QDoubleSpinBox (this);
  speedPOS->setDecimals (1);
  speedPOS->setRange (50.0, 500.0);
  speedPOS->setSingleStep (10.0);
  speedPOS->setValue (options->speed[NAVTRACK_POS]);
  speedPOS->setWrapping (true);
  speedPOS->setToolTip (tr ("Set the speed limit for POS/SBET files (knots)"));
  speedPOS->setWhatsThis (speedPOSText);
  psBoxLayout->addWidget (speedPOS);
  pBoxLayout->addWidget (psBox);


  //Add recent additions to Limits GroupBox / vertical layout

  lboxLayout->addWidget (pBox);


  //Add entire Limits GroupBox to actual page / vertical layout

  vbox->addWidget (lbox);


  //Create new GroupBox

  QGroupBox *yBox = new QGroupBox (tr ("Max Time (in Minutes)"), this);
  QHBoxLayout *yBoxLayout = new QHBoxLayout;
  yBox->setLayout (yBoxLayout);
  yBoxLayout->setSpacing (80);
  

  //Create new QDoubleSpinBox and add to page

  maxTimeForaPoint = new QDoubleSpinBox (this);
  maxTimeForaPoint->setDecimals(1);
  maxTimeForaPoint->setRange(0.50, 60.0);
  maxTimeForaPoint->setSingleStep(0.50);
  maxTimeForaPoint->setWrapping (true);
  maxTimeForaPoint->setToolTip (tr ("Set max time spent before requiring a new point"));
  maxTimeForaPoint->setWhatsThis (maxTimeForaPointText);
  yBoxLayout->addWidget (maxTimeForaPoint);


  //Add entire Max Time GroupBox to actual page / vertical layout

  vbox->addWidget(yBox);


  //Register all QDoubleSpinBox variables so we may read its value from any page in the wizard

  registerField ("maxTimeForaPoint", maxTimeForaPoint, "value", "valueChanged");
  registerField ("headingGSF", headingGSF, "value", "valueChanged");
  registerField ("speedGSF", speedGSF, "value", "valueChanged");
  registerField ("headingPOS", headingPOS, "value", "valueChanged");
  registerField ("speedPOS", speedPOS, "value", "valueChanged");
}
