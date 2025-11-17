#include "object_detection/detected_objects.hpp"
#include "object_detection/detected_image.hpp"
#include "object_detection/mateo.hpp"




void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    //cout << "Image received." << endl;

    Mat pieces = cv_bridge::toCvShare(msg, "bgr8")->image;

    Mat hsv;
    cvtColor(pieces, hsv, COLOR_BGR2HSV);

    Mat mask_rouge, mask_vert, mask_bleu, mask_noir;

    // Masque pour pieces bleu, rouge, verte et croix noir
    Mat mask_rouge1, mask_rouge2;
    inRange(hsv, Scalar(0, 120, 70), Scalar(10, 255, 255), mask_rouge1);
    inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), mask_rouge2);
    mask_rouge = mask_rouge1 | mask_rouge2;

    inRange(hsv, Scalar(40, 70, 70), Scalar(85, 255, 255), mask_vert);

    inRange(hsv, Scalar(90, 150, 70), Scalar(140, 255, 255), mask_bleu);

    inRange(hsv, Scalar(0, 0, 0), Scalar(180, 255, 30), mask_noir); // croix noire

    // application des filtres
    Mat filtre_rouge, filtre_vert, filtre_bleu, filtre_noir;
    bitwise_and(pieces, pieces, filtre_rouge, mask_rouge);
    bitwise_and(pieces, pieces, filtre_vert, mask_vert);
    bitwise_and(pieces, pieces, filtre_bleu, mask_bleu);
    bitwise_and(pieces, pieces, filtre_noir, mask_noir);

    Moments m_rouge = moments(mask_rouge, true);
    if (m_rouge.m00 > 1000) {   // seuil pour éviter le bruit
        int cx_rouge = m_rouge.m10 / m_rouge.m00;
        int cy_rouge = m_rouge.m01 / m_rouge.m00;
        circle(pieces, Point(cx_rouge, cy_rouge), 6, Scalar(0,0,0), -1);
        cout << "Centre rouge: (" << cx_rouge << ", " << cy_rouge << ")\n";
    }
    else
    {
        cout << "Aucune pièce rouge détectée." << endl;
    }


    Moments m_vert = moments(mask_vert, true);
    if (m_vert.m00 > 1000) {
        int cx_vert = m_vert.m10 / m_vert.m00;
        int cy_vert = m_vert.m01 / m_vert.m00;
        cout << "Centre pièce verte : (" << cx_vert << ", " << cy_vert << ")" << endl;
        circle(pieces, Point(cx_vert, cy_vert), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce verte détectée." << endl;
    }

    Moments m_bleu = moments(mask_bleu, true);
    if (m_bleu.m00 > 1000) {
        int cx_bleu = m_bleu.m10 / m_bleu.m00;
        int cy_bleu = m_bleu.m01 / m_bleu.m00;
        cout << "Centre pièce bleue : (" << cx_bleu << ", " << cy_bleu << ")" << endl;
        circle(pieces, Point(cx_bleu, cy_bleu), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce bleue détectée." << endl;
    }

    Moments m_noir = moments(mask_noir, true);
    if (m_noir.m00 > 1000) {
        int cx_noir = m_noir.m10 / m_noir.m00;
        int cy_noir = m_noir.m01 / m_noir.m00;
        cout << "Centre pièce noire : (" << cx_noir << ", " << cy_noir << ")" << endl;
        circle(pieces, Point(cx_noir, cy_noir), 6, Scalar(255, 255, 255), -1);
    }
    else
    {
        cout << "Aucune pièce noire détectée." << endl;
    }

    imshow("Centres des pièces", filtre_rouge);
    waitKey(1);
}

int mateo(int argc, char** argv)
{
    ros::init(argc, argv, "object_detector");
    ros::NodeHandle nh;
    cout << "Mateo node started." << endl;

    namedWindow("camera", WINDOW_AUTOSIZE);

    // Souscription au flux caméra
    ros::Subscriber sub = nh.subscribe("/camera/color/image_raw", 1, imageCallback);


    ros::spin();
    return 0;
}

