/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>
#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>
#include <berryIPartListener.h>
#include <mitkGlobalInteraction.h>
#include <mitkDataStorageEditorInput.h>
#include "berryFileEditorInput.h"

// Qmitk
#include "QmitkDicomEditor.h"
#include "QmitkStdMultiWidget.h"
#include <mitkDicomSeriesReader.h>

// Qt
#include <QCheckBox>
#include <QMessageBox>
#include <QWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>

//CTK
#include <ctkDICOMModel.h>
#include <ctkDICOMAppWidget.h>
#include <ctkDICOMQueryWidget.h>
#include <ctkFileDialog.h>
#include <ctkDICOMQueryRetrieveWidget.h>


const std::string QmitkDicomEditor::EDITOR_ID = "org.mitk.editors.dicomeditor";


QmitkDicomEditor::QmitkDicomEditor()
{
}

QmitkDicomEditor::~QmitkDicomEditor()
{
    delete m_ImportDialog;
}

void QmitkDicomEditor::CreateQtPartControl(QWidget *parent )
{   
    // create GUI widgets from the Qt Designer's .ui file
    m_Controls.setupUi( parent );



    //connections for base controls
    connect(m_Controls.CDButton, SIGNAL(clicked()), this, SLOT(OnFolderCDImport()));
    connect(m_Controls.FolderButton, SIGNAL(clicked()), this, SLOT(OnFolderCDImport()));
    connect(m_Controls.QueryRetrieveButton, SIGNAL(clicked()), this, SLOT(OnQueryRetrieve()));
    connect(m_Controls.LocalStorageButton, SIGNAL(clicked()), this, SLOT(OnLocalStorage()));

    //Initialize import widget
    m_ImportDialog = new ctkFileDialog();
    QCheckBox* importCheckbox = new QCheckBox("Copy on import", m_ImportDialog);
    m_ImportDialog->setBottomWidget(importCheckbox);
    m_ImportDialog->setFileMode(QFileDialog::Directory);
    m_ImportDialog->setLabelText(QFileDialog::Accept,"Import");
    m_ImportDialog->setWindowTitle("Import DICOM files from directory ...");
    m_ImportDialog->setWindowModality(Qt::ApplicationModal);
    connect(m_ImportDialog, SIGNAL(fileSelected(QString)),this,SLOT(OnImportDirectory(QString)));

    //m_Controls.ExternalDataTreeView->setSortingEnabled(true);
    //m_Controls.ExternalDataTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //m_Controls.ExternalDataTreeView->setModel();

    //m_Controls.InternalDataTreeView->setSortingEnabled(true);
    //m_Controls.InternalDataTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    //m_Controls.InternalDataTreeView->setModel();
    //connect( m_Controls.m_ctkDICOMAppWidget, SIGNAL(seriesDoubleClicked( QModelIndex )), this, SLOT(onSeriesModelSelected( QModelIndex )) );


}

void QmitkDicomEditor::OnSelectionChanged( std::vector<mitk::DataNode*> nodes )
{
    // iterate all selected objects, adjust warning visibility
    for( std::vector<mitk::DataNode*>::iterator it = nodes.begin();
        it != nodes.end();
        ++it )
    {
        mitk::DataNode::Pointer node = *it;
        if( node.IsNotNull() && dynamic_cast<mitk::Image*>(node->GetData()) )
        {
            //m_Controls.labelWarning->setVisible( false );
            //m_Controls.buttonPerformImageProcessing->setEnabled( true );
            return;
        }
    }

    //m_Controls.labelWarning->setVisible( true );
    //m_Controls.buttonPerformImageProcessing->setEnabled( false );
}

void QmitkDicomEditor::OnSeriesModelDoubleClicked(QModelIndex index){
    QModelIndex studyIndex = index.parent();
    QModelIndex seriesIndex = index;

    //ctkDICOMModel* model = const_cast<ctkDICOMModel*>(qobject_cast<const ctkDICOMModel*>(index.model()));

    //if(model)
    //{
    //    model->fetchMore(seriesIndex);

    //    int imageCount = model->rowCount(seriesIndex);

    //    MITK_INFO<< "Series Index:"<< imageCount << "\n";

    //    QString filePath = m_Controls.m_ctkDICOMAppWidget->databaseDirectory() +
    //        "/dicom/" + model->data(studyIndex ,ctkDICOMModel::UIDRole).toString() + "/";

    //    MITK_INFO << "filepath: "<< filePath.toStdString() << "\n";

    //    QString series_uid = model->data(seriesIndex ,ctkDICOMModel::UIDRole).toString();

    //    MITK_INFO << "series_uid: " << series_uid.toStdString() << "\n";

    //    if(QFile(filePath).exists())
    //    {
    //        filePath.replace(0,1,"");
    //        QString absolutFilePath("C:/home/bauerm/bin/MITK/MITK-build/Applications/ExtApp/ctkDICOM-Database/dicom/");
    //        absolutFilePath.append(model->data(studyIndex ,ctkDICOMModel::UIDRole).toString());
    //        absolutFilePath.append("/");
    //        //add all fienames from series to strin container
    //        mitk::DicomSeriesReader::StringContainer names = mitk::DicomSeriesReader::GetSeries(absolutFilePath.toStdString(),series_uid.toStdString());
    //        mitk::DataNode::Pointer node = mitk::DicomSeriesReader::LoadDicomSeries(names, true, true);

    //        if (node.IsNull())
    //        {
    //            MITK_ERROR << "Could not load series: " << series_uid.toStdString();
    //        }
    //        else
    //        {
    //            node->SetName(series_uid.toStdString());
    //            this->GetDefaultDataStorage()->Add(node);

    //            this->GetActiveStdMultiWidget();

    //            mitk::RenderingManager::GetInstance()->InitializeViews(node->GetData()->GetTimeSlicedGeometry(), mitk::RenderingManager::REQUEST_UPDATE_ALL, true);
    //            mitk::RenderingManager::GetInstance()->RequestUpdateAll();
    //        }

    //    }
    //}
}

void QmitkDicomEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
    //if (input.Cast<berry::FileEditorInput>().IsNull())
    //   throw berry::PartInitException("Invalid Input: Must be FileEditorInput");

    this->SetSite(site);
    this->SetInput(input);
}

void QmitkDicomEditor::SetFocus()
{
}

berry::IPartListener::Events::Types QmitkDicomEditor::GetPartEventTypes() const
{
    return Events::CLOSED | Events::HIDDEN | Events::VISIBLE;
}

void QmitkDicomEditor::OnFolderCDImport()
{
    m_ImportDialog->show();
    m_ImportDialog->raise();

}

void QmitkDicomEditor::OnQueryRetrieve()
{
    m_Controls.stackedWidget->setCurrentIndex(2);
}

void QmitkDicomEditor::OnLocalStorage()
{
    m_Controls.stackedWidget->setCurrentIndex(0);
}

void QmitkDicomEditor::OnImportDirectory(QString directory)
{
    if (QDir(directory).exists())
    {
        QCheckBox* copyOnImport = qobject_cast<QCheckBox*>(m_ImportDialog->bottomWidget());
        QString targetDirectory;
        if (copyOnImport->isChecked())
        {
            //targetDirectory = d->DICOMDatabase->databaseDirectory();
            MBI_DEBUG<<directory.toStdString();
            
        }else{
            m_Controls.stackedWidget->setCurrentIndex(1);
        }
        //d->DICOMIndexer->addDirectory(*d->DICOMDatabase,directory,targetDirectory);
        //d->DICOMModel.reset();
    }

}
