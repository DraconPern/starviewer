/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPROCESSIMAGESINGLETON_H
#define UDGPROCESSIMAGESINGLETON_H

#include <QString>
#include <QList>
#include <cond.h>

class QSemaphore;

namespace udg {

class ProcessImage;
class Image;
class DICOMTagReader;

/** Aquesta és una classe singleton que conté una col.leció d'objectes ProcessImage. La classe és necessaria per poder descarregar estudis simultàniament ja que per cada estudi que és descarrega, a través del seu studyUID té associat un ProcessImage, que és que s'encarrega de dur a terme les accions pertinents després de la descèrrega de cada imatge.
 *
@author marc
 */
class ProcessImageSingleton{
public:

    /** retorna una instancia de l'objecte
     * @return instancia de l'objecte
     */
    static ProcessImageSingleton *getProcessImageSingleton();

    /** Afegeix un nou objecte ProcessImage, que s'encarregarrà de processar la informació de cada imatge descarregada
     * @param UID de l'estudi
     * @param Objecte processimage que tractarà la descarrega d'imatges
     */
    void addNewProcessImage( QString , ProcessImage * );

    /** buscar l'objecte processimage que s'encarrega de gestionar la descarrega de l'estudi studyUID, per processar la imatge
     * @param UID de l'estudi que ha produit l'error
     * @param imatge descarregada
     */
    void process(const QString &studyUID, Image *image);

    ///Processa el DicomTagReader
    void process(const QString &studyUID, DICOMTagReader *dicomTagReader);

    /** buscar l'objecte processimage que s'encarrega de gestionar la descarrega de l'estudi studyUID, per notificar l'error
     * @param UID de l'estudi que ha produit l'error
     */
    void setError( QString studyUID );

    /** esborra el ProcessImage de la llista
     * @param UID del Process Image de l'estudi a esborrar
     */
    bool delProcessImage( QString );

    /** Estableix el path on s'han de guardar les imatges de la caché
     * @param path de la cache on es guarden les imatges
     */
    void setPath( QString );

    /** retorna el path de la caché on s'han de guardar les imatges
     * @return path de la cache
     */
    QString getPath();

    /// destructor de la classe
    ~ProcessImageSingleton();

private:

    static ProcessImageSingleton* pInstance;

    struct SingletonProcess
    {
        ProcessImage *imgProcess;//ha de ser un punter pq sino creari un objecte del pare, cridaria les funcions del pare i no dels fills
        QString studyUID;
    };

    QList<SingletonProcess> m_listProcess;

    QSemaphore *m_semaphore;
    QString m_imagePath;

    /// Constructor de la classe
    ProcessImageSingleton();
};

};

#endif