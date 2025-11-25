#include "object_detection/detected_objects.hpp"
#include "object_detection/ObjectData.h"
#include "object_detection/Piece.h"

#define OFFSET_X 25
#define OFFSET_Y 50 

#define PX_PER_CM 24

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

    // Declaration position
    float cx_rouge = 0;
    float cy_rouge = 0;
    float cx_rouge_cm = 0;
    float cy_rouge_cm = 0;

    if (presence_rouge) {   // seuil pour éviter le bruit

        // Position en pixel
        cx_rouge = m_rouge.m10 / m_rouge.m00;
        cy_rouge = m_rouge.m01 / m_rouge.m00;

        // Position en cm
        cx_rouge_cm = (cx_rouge - OFFSET_X) / PX_PER_CM;
        cy_rouge_cm = (cy_rouge + OFFSET_Y) / PX_PER_CM;

        circle(pieces, Point(cx_rouge, cy_rouge), 6, Scalar(0,0,0), -1);
        cout << "Centre rouge: (" << cx_rouge << ", " << cy_rouge << ")\n";
    }
    else
    {
        cout << "Aucune pièce rouge détectée." << endl;
    }

    Moments m_vert = moments(mask_vert, true);
    presence_vert = (m_vert.m00 > 1000);

    // Declaration position
    float cx_vert = 0;
    float cy_vert = 0;
    float cx_vert_cm = 0;
    float cy_vert_cm = 0;

    if (presence_vert) {

        // Position en pixel
        cx_vert = m_vert.m10 / m_vert.m00;
        cy_vert = m_vert.m01 / m_vert.m00;

        // Position en cm
        cx_vert_cm = (cx_vert - OFFSET_X) / PX_PER_CM;
        cy_vert_cm = (cy_vert + OFFSET_Y) / PX_PER_CM;

        cout << "Centre pièce verte : (" << cx_vert << ", " << cy_vert << ")" << endl;
        circle(pieces, Point(cx_vert, cy_vert), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce verte détectée." << endl;
    }

    Moments m_bleu = moments(mask_bleu, true);
    presence_bleu = (m_bleu.m00 > 1000);

    // Declaration position
    float cx_bleu = 0;
    float cy_bleu = 0;
    float cx_bleu_cm = 0;
    float cy_bleu_cm = 0;

    if (presence_bleu) {

        // Position en pixel
        cx_bleu = m_bleu.m10 / m_bleu.m00;
        cy_bleu = m_bleu.m01 / m_bleu.m00;

        // Position en cm
        cx_bleu_cm = (cx_bleu - OFFSET_X) / PX_PER_CM;
        cy_bleu_cm = (cy_bleu + OFFSET_Y) / PX_PER_CM;

        cout << "Centre pièce bleue : (" << cx_bleu << ", " << cy_bleu << ")" << endl;
        circle(pieces, Point(cx_bleu, cy_bleu), 6, Scalar(0, 0, 0), -1);
    }
    else
    {
        cout << "Aucune pièce bleue détectée." << endl;
    }

    Moments m_noir = moments(mask_noir, true);
    presence_noir = (m_noir.m00 > 1000);

    // Declaration position
    float cx_noir = 0;
    float cy_noir = 0;
    float cx_noir_cm = 0;
    float cy_noir_cm = 0;

    if (presence_noir) {

        // Position en pixel
        cx_noir = m_noir.m10 / m_noir.m00;
        cy_noir = m_noir.m01 / m_noir.m00;

        // Position en cm
        cx_noir_cm = (cx_noir - OFFSET_X) / PX_PER_CM;
        cy_noir_cm = (cy_noir + OFFSET_Y) / PX_PER_CM;

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

        data.bleu.x = cx_bleu_cm;
        data.bleu.y = cy_bleu_cm;
        data.bleu.present = presence_bleu;
        data.rouge.x = cx_rouge_cm;
        data.rouge.y = cy_rouge_cm;
        data.rouge.present = presence_rouge;
        data.vert.x = cx_vert_cm;
        data.vert.y = cy_vert_cm;
        data.vert.present = presence_vert;
        data.croix.x = cx_noir_cm;
        data.croix.y = cy_noir_cm;
        data.croix.present = presence_noir;

        data_camera.publish(data);
        sleep(1);
    }
}

void readyCallback(const std_msgs::Bool::ConstPtr& msg)
{
    readytoProcess = msg->data;
    cout << "Ready to process: " << readytoProcess << endl;
}

int main(int argc, char** argv)
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



