#include "dicomanonymizer.h"

#include <gdcmGlobal.h>
#include <gdcmUIDGenerator.h>
#include <gdcmReader.h>
#include <gdcmWriter.h>
#include <gdcmDefs.h>
#include <QCoreApplication>
#include <QDir>
#include <dcuid.h>

#include "logging.h"

namespace udg {

DICOMAnonymizer::DICOMAnonymizer()
{
    initializeGDCM();

    m_replacePatientIDInsteadOfRemove = false;
    m_replaceStudyIDInsteadOfRemove = false;
    m_removePritaveTags = false;
}

DICOMAnonymizer::~DICOMAnonymizer()
{
    delete gdcmGlobalInstance;
}

void DICOMAnonymizer::setReplacePatientIDInsteadOfRemove(bool replace)
{
    m_replacePatientIDInsteadOfRemove = replace;
}

bool DICOMAnonymizer::getReplacePatientIDInsteadOfRemove()
{
    return m_replacePatientIDInsteadOfRemove;
}


void DICOMAnonymizer::setReplaceStudyIDInsteadOfRemove(bool replace)
{
    m_replaceStudyIDInsteadOfRemove = replace;
}

bool DICOMAnonymizer::getReplaceStudyIDInsteadOfRemove()
{
    return m_replaceStudyIDInsteadOfRemove;
}

void DICOMAnonymizer::setRemovePrivateTags(bool removePritaveTags)
{
    m_removePritaveTags = removePritaveTags;
}

bool DICOMAnonymizer::getRemovePrivateTags()
{
    return m_removePritaveTags;
}

bool DICOMAnonymizer::initializeGDCM()
{
    gdcmAnonymizer = new gdcm::gdcmAnonymizerStarviewer();
    gdcmGlobalInstance = &gdcm::Global::GetInstance();

    //Indiquem el directori on pot trobar el fitxer part3.xml que �s un diccionari DICOM.
    //TODO: On posem el fitxer part3.xml
    gdcmGlobalInstance->Prepend(qPrintable(QCoreApplication::applicationDirPath()));

    //Carrega el fitxer part3.xml
    if (!gdcmGlobalInstance->LoadResourcesFiles())
    {
        ERROR_LOG("No s'ha trobat el fitxer part3.xml a " + QCoreApplication::applicationDirPath());
        return false;
    }

    const gdcm::Defs &defs = gdcmGlobalInstance->GetDefs(); (void)defs;
    //TODO:utilitzem el UID de dcmtk haur�em de tenir el nostre propi aix� tamb� passa a VolumeBuilderFromCaptures
    //TODO:Treure quan tot funcioni
    if (!gdcm::UIDGenerator::IsValid(SITE_UID_ROOT))
    {
        DEBUG_LOG("UID NO V�LID");
    }

    gdcm::UIDGenerator::SetRoot(SITE_UID_ROOT);

    DEBUG_LOG("Inicialitzat");

    return true;
}

bool DICOMAnonymizer::anonymyzeDICOMFilesDirectory(QString directoryPath)
{
    QDir directory;
    directory.setPath(directoryPath);

    foreach(QFileInfo entryInfo, directory.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files))
    {
        if (entryInfo.isDir())
        {
            anonymyzeDICOMFilesDirectory(entryInfo.absoluteFilePath());
        }
        else
        {
            anonymizeDICOMFile(entryInfo.absoluteFilePath(), entryInfo.absoluteFilePath());
        }
    }

    //TODO:Retornar valor correcte
    return true;
}

//TODO:Testejar que el frameofreference, studyInstanceUID, .. tot sigui coherent. 
bool DICOMAnonymizer::anonymizeDICOMFile(QString inputPathFile, QString outputPathFile)
{
    gdcm::Reader gdcmReader;

    gdcmReader.SetFileName(qPrintable(inputPathFile));

    if (!gdcmReader.Read())
    {
        DEBUG_LOG("No s'ha trobat el fitxer a anonimitzar " + inputPathFile);
        return false;
    }

    gdcm::File &gdcmFile = gdcmReader.GetFile();

    gdcm::MediaStorage gdcmMediaStorage;
    gdcmMediaStorage.SetFromFile(gdcmFile);
    if (!gdcm::Defs::GetIODNameFromMediaStorage(gdcmMediaStorage))
    {
        //TODO:Testejar que retorn el correcte
        DEBUG_LOG(QString("Media storage type del fitxer no suportat: %1").arg(gdcmMediaStorage.GetString()));
        return false;
    }

    gdcmAnonymizer->SetFile(gdcmFile);

    if (!gdcmAnonymizer->BasicApplicationLevelConfidentialityProfile(true))
    {
        DEBUG_LOG("No s'ha pogut anonimitzar el fitxer " + inputPathFile);
    }

    std::string newUIDValue = "";
    gdcm::DataElement copyNew = gdcmFile.GetDataSet().GetDataElement(gdcm::Tag(0x0008, 0x0018));

    gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0010), "Anonymous"); //Nom pacient
    
    if (getReplacePatientIDInsteadOfRemove())
    {
        gdcmAnonymizer->Replace(gdcm::Tag(0x0010, 0x0020), "99999"); //ID Pacient
    }

    if (getReplaceStudyIDInsteadOfRemove())
    {
        gdcmAnonymizer->Replace(gdcm::Tag(0x0020, 0x0010), "99999"); //ID Estudi
    }

/*    if (getRemovePrivateTags())
    {
        gdcmAnonymizer->RemovePrivateTags();
    }
*/

    //TODO: Perqu� serveix?
    gdcm::FileMetaInformation gdcmFileMetaInformation = gdcmFile.GetHeader();
    gdcmFileMetaInformation.Clear();

    gdcm::Writer gdcmWriter;
    gdcmWriter.SetFileName(qPrintable(outputPathFile));
    gdcmWriter.SetFile(gdcmFile);
    if (!gdcmWriter.Write())
    {
        DEBUG_LOG("No s'ha pogut generar el fitxer anonimitzat " + inputPathFile + " a " + outputPathFile);
        return false;
    }

      /*

  if( !writer.Write() )
    {
    std::cerr << "Could not Write : " << outfilename << std::endl;
    if( strcmp(filename,outfilename) != 0 )
      {
      gdcm::System::RemoveFile( outfilename );
      }
    else
      {
      std::cerr << "gdcmanon just corrupted: " << filename << " for you (data lost)." << std::endl;
      }

    return false;
    }*/
  return true;

}

}