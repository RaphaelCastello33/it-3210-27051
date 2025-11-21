#include "object_detection/detected_objects.hpp"
#include "object_detection/detected_image.hpp"
#include "object_detection/mateo.hpp"


#include "object_detection/ObjectData.h"
#include "object_detection/Piece.h"

#define OFFSET_X 25
#define OFFSET_Y 50 

bool readytoProcess = false;
ros::Publisher ready_pub;
ros::Publisher data_camera;

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    std_msgs::Bool ready_msg;
    object_detection::ObjectData data;


    cout << "Image received." << endl;

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

    bool presence_rouge, presence_vert, presence_bleu, presence_noir;

    Moments m_rouge = moments(mask_rouge, true);
    presence_rouge = (m_rouge.m00 > 1000);
    float cx_rouge = 0;
    float cy_rouge = 0;
    float cx_rouge_cm = 0;
    float cy_rouge_cm = 0;
    if (presence_rouge) {   // seuil pour éviter le bruit
        cx_rouge = m_rouge.m10 / m_rouge.m00 - OFFSET_X;
        cy_rouge = m_rouge.m01 / m_rouge.m00 + OFFSET_Y;
        circle(pieces, Point(cx_rouge, cy_rouge), 6, Scalar(0,0,0), -1);
        cout << "Centre rouge: (" << cx_rouge << ", " << cy_rouge << ")\n";
    }
    else
    {
        cout << "Aucune pièce rouge détectée." << endl;
    }

    Moments m_vert = moments(mask_vert, true);
    presence_vert = (m_vert.m00 > 1000);
    float cx_vert = 0;
    float cy_vert = 0;
    float cx_vert_cm = 0;
    float cy_vert_cm = 0;
    if (presence_vert) {
        cx_vert = m_vert.m10 / m_vert.m00 - OFFSET_X;
        cy_vert = m_vert.m01 / m_vert.m00 + OFFSET_Y;
        cout << "Centre pièce verte : (" << cx_vert << ", " << cy_vert << ")" << endl;
        circle(pieces, Point(cx_vert, cy_vert), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce verte détectée." << endl;
    }

    Moments m_bleu = moments(mask_bleu, true);
    presence_bleu = (m_bleu.m00 > 1000);
    float cx_bleu = 0;
    float cy_bleu = 0;
    float cx_bleu_cm = 0;
    float cy_bleu_cm = 0;
    if (presence_bleu) {
        cx_bleu = m_bleu.m10 / m_bleu.m00 - OFFSET_X;
        cy_bleu = m_bleu.m01 / m_bleu.m00 + OFFSET_Y;
        cout << "Centre pièce bleue : (" << cx_bleu << ", " << cy_bleu << ")" << endl;
        circle(pieces, Point(cx_bleu, cy_bleu), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce bleue détectée." << endl;
    }

    Moments m_noir = moments(mask_noir, true);
    presence_noir = (m_noir.m00 > 1000);
    float cx_noir = 0;
    float cy_noir = 0;
    float cx_noir_cm = 0;
    float cy_noir_cm = 0;
    if (presence_noir) {
        int cx_noir = m_noir.m10 / m_noir.m00 - OFFSET_X;
        int cy_noir = m_noir.m01 / m_noir.m00 + OFFSET_Y;
        cout << "Centre pièce noire : (" << cx_noir << ", " << cy_noir << ")" << endl;
        circle(pieces, Point(cx_noir, cy_noir), 6, Scalar(255, 255, 255), -1);
    }
    else
    {
        cout << "Aucune pièce noire détectée." << endl;
    }

    imshow("Centres des pièces", pieces);
    waitKey(1);

    if(readytoProcess)
    {
        cout << "Pub data" << endl;
        readytoProcess = false; // reset flag
        ready_msg.data = false;
        ready_pub.publish(ready_msg);

        data.bleu.x = cx_bleu;
        data.bleu.y = cy_bleu;
        data.bleu.present = presence_bleu;
        data.rouge.x = cx_rouge;
        data.rouge.y = cy_rouge;
        data.rouge.present = presence_rouge;
        data.vert.x = cx_vert;
        data.vert.y = cy_vert;
        data.vert.present = presence_vert;
        data.noir.x = cx_noir;
        data.noir.y = cy_noir;
        data.noir.present = presence_noir;

        data_camera.publish(data);
        sleep(1);
    }
}

void readyCallback(const std_msgs::Bool::ConstPtr& msg)
{
    readytoProcess = msg->data;
    cout << "Ready to process: " << readytoProcess << endl;
}

int mateo(int argc, char** argv)
{
    ros::init(argc, argv, "object_detector");
    ros::NodeHandle nh;

    cout << "Mateo node started." << endl;

    namedWindow("camera", WINDOW_AUTOSIZE);

    //publishers

    ready_pub = nh.advertise<std_msgs::Bool>("/request_object_data", 1);
    data_camera = nh.advertise<object_detection::ObjectData>("/object_data", 1);

    //suscribers

    ros::Subscriber ready_sub = nh.subscribe("/request_object_data", 1, readyCallback);
    ros::Subscriber sub = nh.subscribe("/camera/color/image_raw", 1, imageCallback);

    ros::spin();
    return 0;
}

