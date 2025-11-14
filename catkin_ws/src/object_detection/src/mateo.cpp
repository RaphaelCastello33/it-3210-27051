#include "object_detection/detected_objects.hpp"
#include "object_detection/detected_image.hpp"
#include "object_detection/mateo.hpp"


int mateo()
{
    cout << "Hello world" << endl;

    string pkg_path = ros::package::getPath("object_detection");
    string img_path = pkg_path + "/src/photo_pieces_croix.jpg";

    Mat pieces = imread(img_path, IMREAD_UNCHANGED);
    if (pieces.empty()) {
        cerr << "Erreur de chargement de l'image !" << endl;
        return -1;
    }

    cout << "Type: " << pieces.type() << endl;
    cout << "Canaux: " << pieces.channels() << endl;

    // namedWindow("pieces", WINDOW_NORMAL);
    // imshow("pieces", pieces);

    Mat hsv;
    cvtColor(pieces, hsv, COLOR_BGR2HSV);

    Mat mask_rouge, mask_vert, mask_bleu, mask_noir;

    // Masque pour pieces bleu, rouge, verte et croix noir
    Mat mask_rouge1, mask_rouge2;
    inRange(hsv, Scalar(0, 120, 70), Scalar(10, 255, 255), mask_rouge1);
    inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), mask_rouge2);
    mask_rouge = mask_rouge1 | mask_rouge2;
    inRange(hsv, Scalar(35, 100, 70), Scalar(85, 255, 255), mask_vert);
    inRange(hsv, Scalar(90, 100, 70), Scalar(130, 255, 255), mask_bleu);
    inRange(hsv, Scalar(0, 0, 0), Scalar(180, 255, 30), mask_noir); // croix noire


    // --- Appliquer les masques pour isoler les couleurs ---
    Mat filtre_rouge, filtre_vert, filtre_bleu, filtre_noir;
    bitwise_and(pieces, pieces, filtre_rouge, mask_rouge);
    bitwise_and(pieces, pieces, filtre_vert, mask_vert);
    bitwise_and(pieces, pieces, filtre_bleu, mask_bleu);
    bitwise_and(pieces, pieces, filtre_noir, mask_noir);

    // --- Afficher les résultats ---
    // namedWindow("Rouge", WINDOW_NORMAL);
    // imshow("Rouge", filtre_rouge);

    // namedWindow("Vert", WINDOW_NORMAL);
    // imshow("Vert", filtre_vert);

    // namedWindow("Bleu", WINDOW_NORMAL);
    // imshow("Bleu", filtre_bleu);

    Moments m_rouge = moments(mask_rouge, true);
    int cx_rouge = m_rouge.m10 / m_rouge.m00;
    int cy_rouge = m_rouge.m01 / m_rouge.m00;
    cout << "Centre pièce rouge : (" << cx_rouge << ", " << cy_rouge << ")" << endl;

    Moments m_vert = moments(mask_vert, true);
    int cx_vert = m_vert.m10 / m_vert.m00;
    int cy_vert = m_vert.m01 / m_vert.m00;
    cout << "Centre pièce verte : (" << cx_vert << ", " << cy_vert << ")" << endl;

    Moments m_bleu = moments(mask_bleu, true);
    int cx_bleu = m_bleu.m10 / m_bleu.m00;
    int cy_bleu = m_bleu.m01 / m_bleu.m00;
    cout << "Centre pièce bleue : (" << cx_bleu << ", " << cy_bleu << ")" << endl;

    Moments m_noir = moments(mask_noir, true);
    int cx_noir = m_noir.m10 / m_noir.m00;
    int cy_noir = m_noir.m01 / m_noir.m00;
    cout << "Centre pièce noire : (" << cx_noir << ", " << cy_noir << ")" << endl;

    if (m_rouge.m00 != 0) {
        circle(pieces, Point(cx_rouge, cy_rouge), 6, Scalar(0, 0, 0), -1);
    }

    if (m_vert.m00 != 0) {
        circle(pieces, Point(cx_vert, cy_vert), 6, Scalar(0, 0, 0), -1); 
    }

    if (m_bleu.m00 != 0) {
        circle(pieces, Point(cx_bleu, cy_bleu), 6, Scalar(0, 0, 0), -1); 
    }

    if (m_noir.m00 != 0) {
        circle(pieces, Point(cx_noir, cy_noir), 6, Scalar(255, 255, 255), -1); 
    }

    namedWindow("Centres des pièces", WINDOW_NORMAL);
    imshow("Centres des pièces", pieces);

    waitKey(0);
    return 0;
}

