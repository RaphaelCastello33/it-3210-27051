#include "object_detection/detected_objects.hpp"
#include "object_detection/detected_image.hpp"
#include "object_detection/mateo.hpp"


int mateo()
{
    cout << "Hello world" << endl;

    // Charger les images
    string pkg_path = ros::package::getPath("object_detection");
    string img_path = pkg_path + "/src/image.jpg";

    Mat pieces = imread(img_path);

    // Vérification que l'image est bien chargée
    if (pieces.empty()) {
        cerr << "Erreur de chargement des images!" << endl;
        return -1;
    }

    namedWindow("pieces ", WINDOW_NORMAL | WINDOW_GUI_EXPANDED);
    imshow("pieces", pieces);

    // Convertir l'image 'pieces' en YCbCr
    Mat pieces_ycbcr;
    cvtColor(pieces, pieces_ycbcr, COLOR_BGR2YCrCb);

    // Diviser les canaux de l'image 'pieces'
    Mat pieces_r, pieces_g, pieces_b;
    vector<Mat> channels;
    split(pieces, channels); // split pieces image into R, G, B channels

    pieces_r = channels[2]; // Canal Rouge (BGR -> R)
    pieces_g = channels[1]; // Canal Vert (BGR -> G)
    pieces_b = channels[0]; // Canal Bleu (BGR -> B)

    // Afficher les canaux de l'image 'pieces'
    namedWindow("pieces - Red Channel", WINDOW_NORMAL);
    imshow("pieces - Red Channel", pieces_r);

    namedWindow("pieces - Green Channel", WINDOW_NORMAL);
    imshow("pieces - Green Channel", pieces_g);

    namedWindow("pieces - Blue Channel", WINDOW_NORMAL);
    imshow("pieces - Blue Channel", pieces_b);

    //waitKey(0);

    // Masque binaire sur le canal R
    Mat pieces_r_mask;
    threshold(pieces_r, pieces_r_mask, 0.03 * 255, 255, THRESH_BINARY);

    // Afficher le masque
    namedWindow("Red Channel Mask", WINDOW_NORMAL);
    imshow("Red Channel Mask", pieces_r_mask);
    waitKey(0);

    // Convertir 'pieces' en YCbCr
    Mat pieces_ycbcr_image;
    cvtColor(pieces, pieces_ycbcr_image, COLOR_BGR2YCrCb);

    // Séparer les canaux Y, Cb, Cr
    Mat pieces_y, pieces_cb, pieces_cr;
    vector<Mat> channels_ycbcr;
    split(pieces_ycbcr_image, channels_ycbcr);

    pieces_y = channels_ycbcr[0]; // Canal Y
    pieces_cb = channels_ycbcr[1]; // Canal Cb
    pieces_cr = channels_ycbcr[2]; // Canal Cr

    // Afficher les canaux Y, Cb, Cr
    namedWindow("Y Channel", WINDOW_NORMAL);
    imshow("Y Channel", pieces_y);

    namedWindow("Cb Channel", WINDOW_NORMAL);
    imshow("Cb Channel", pieces_cb);

    namedWindow("Cr Channel", WINDOW_NORMAL);
    imshow("Cr Channel", pieces_cr);

    waitKey(0);
    return 0;
}

