#!/bin/bash

sed '/AL/d' ./negatives.txt > negatives_AL.txt 
sed '/AR/d' ./negatives.txt > negatives_AR.txt
sed '/BL/d' ./negatives.txt > negatives_BL.txt
sed '/BR/d' ./negatives.txt > negatives_BR.txt 
sed '/CL/d' ./negatives.txt > negatives_CL.txt 
sed '/CR/d' ./negatives.txt > negatives_CR.txt 
sed '/DL/d' ./negatives.txt > negatives_DL.txt 
sed '/DR/d' ./negatives.txt > negatives_DR.txt 
sed '/EL/d' ./negatives.txt > negatives_EL.txt 
sed '/ER/d' ./negatives.txt > negatives_ER.txt 
sed '/FL/d' ./negatives.txt > negatives_FL.txt 
sed '/FR/d' ./negatives.txt > negatives_FR.txt 
sed '/GL/d' ./negatives.txt > negatives_GL.txt 
sed '/GR/d' ./negatives.txt > negatives_GR.txt 
sed '/HL/d' ./negatives.txt > negatives_HL.txt 
sed '/HR/d' ./negatives.txt > negatives_HR.txt 
sed '/IL/d' ./negatives.txt > negatives_IL.txt 
sed '/IR/d' ./negatives.txt > negatives_IR.txt 
sed '/KL/d' ./negatives.txt > negatives_KL.txt 
sed '/KR/d' ./negatives.txt > negatives_KR.txt 
sed '/LL/d' ./negatives.txt > negatives_LL.txt 
sed '/LR/d' ./negatives.txt > negatives_LR.txt 
sed '/ML/d' ./negatives.txt > negatives_ML.txt 
sed '/MR/d' ./negatives.txt > negatives_MR.txt 
sed '/NL/d' ./negatives.txt > negatives_NL.txt 
sed '/NR/d' ./negatives.txt > negatives_NR.txt 
sed '/OL/d' ./negatives.txt > negatives_OL.txt 
sed '/OR/d' ./negatives.txt > negatives_OR.txt 
sed '/PL/d' ./negatives.txt > negatives_PL.txt 
sed '/PR/d' ./negatives.txt > negatives_PR.txt 
sed '/QL/d' ./negatives.txt > negatives_QL.txt 
sed '/QR/d' ./negatives.txt > negatives_QR.txt 
sed '/RL/d' ./negatives.txt > negatives_RL.txt 
sed '/RR/d' ./negatives.txt > negatives_RR.txt 
sed '/SL/d' ./negatives.txt > negatives_SL.txt 
sed '/SR/d' ./negatives.txt > negatives_SR.txt 
sed '/TL/d' ./negatives.txt > negatives_TL.txt 
sed '/TR/d' ./negatives.txt > negatives_TR.txt 
sed '/UL/d' ./negatives.txt > negatives_UL.txt 
sed '/UR/d' ./negatives.txt > negatives_UR.txt 
sed '/VL/d' ./negatives.txt > negatives_VL.txt 
sed '/VR/d' ./negatives.txt > negatives_VR.txt 
sed '/WL/d' ./negatives.txt > negatives_WL.txt 
sed '/WR/d' ./negatives.txt > negatives_WR.txt 
sed '/XL/d' ./negatives.txt > negatives_XL.txt 
sed '/XR/d' ./negatives.txt > negatives_XR.txt 
sed '/YL/d' ./negatives.txt > negatives_YL.txt 
sed '/YR/d' ./negatives.txt > negatives_YR.txt 


opencv_createsamples -img ./positive_images/AL.jpg -num 100 -vec ./samples/samples_AL.vec -bg negatives_AL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 51 -h 56 
opencv_createsamples -img ./positive_images/AR.jpg -num 100 -vec ./samples/samples_AR.vec -bg negatives_AR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 51 -h 56 
opencv_createsamples -img ./positive_images/BL.jpg -num 100 -vec ./samples/samples_BL.vec -bg negatives_BL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 51 -h 93 
opencv_createsamples -img ./positive_images/BR.jpg -num 100 -vec ./samples/samples_BR.vec -bg negatives_BR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 47 -h 82 
opencv_createsamples -img ./positive_images/CL.jpg -num 100 -vec ./samples/samples_CL.vec -bg negatives_CL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 67 -h 77 
opencv_createsamples -img ./positive_images/CR.jpg -num 100 -vec ./samples/samples_CR.vec -bg negatives_CR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 48 -h 56 
opencv_createsamples -img ./positive_images/DL.jpg -num 100 -vec ./samples/samples_DL.vec -bg negatives_DL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 49 -h 81 
opencv_createsamples -img ./positive_images/DR.jpg -num 100 -vec ./samples/samples_DR.vec -bg negatives_DR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 43 -h 84 
opencv_createsamples -img ./positive_images/EL.jpg -num 100 -vec ./samples/samples_EL.vec -bg negatives_EL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 53 -h 69 
opencv_createsamples -img ./positive_images/ER.jpg -num 100 -vec ./samples/samples_ER.vec -bg negatives_ER.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 47 -h 66 
opencv_createsamples -img ./positive_images/FL.jpg -num 100 -vec ./samples/samples_FL.vec -bg negatives_FL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 72 -h 101
opencv_createsamples -img ./positive_images/FR.jpg -num 100 -vec ./samples/samples_FR.vec -bg negatives_FR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 56 -h 87 
opencv_createsamples -img ./positive_images/GL.jpg -num 100 -vec ./samples/samples_GL.vec -bg negatives_GL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 94 -h 76 
opencv_createsamples -img ./positive_images/GR.jpg -num 100 -vec ./samples/samples_GR.vec -bg negatives_GR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 69 -h 61 
opencv_createsamples -img ./positive_images/HL.jpg -num 100 -vec ./samples/samples_HL.vec -bg negatives_HL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 85 -h 62 
opencv_createsamples -img ./positive_images/HR.jpg -num 100 -vec ./samples/samples_HR.vec -bg negatives_HR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 91 -h 48 
opencv_createsamples -img ./positive_images/IL.jpg -num 100 -vec ./samples/samples_IL.vec -bg negatives_IL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 64 -h 90 
opencv_createsamples -img ./positive_images/IR.jpg -num 100 -vec ./samples/samples_IR.vec -bg negatives_IR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 51 -h 79 
opencv_createsamples -img ./positive_images/KL.jpg -num 100 -vec ./samples/samples_KL.vec -bg negatives_KL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 68 -h 92 
opencv_createsamples -img ./positive_images/KR.jpg -num 100 -vec ./samples/samples_KR.vec -bg negatives_KR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 56 -h 88 
opencv_createsamples -img ./positive_images/LL.jpg -num 100 -vec ./samples/samples_LL.vec -bg negatives_LL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 79 -h 83 
opencv_createsamples -img ./positive_images/LR.jpg -num 100 -vec ./samples/samples_LR.vec -bg negatives_LR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 73 -h 85 
opencv_createsamples -img ./positive_images/ML.jpg -num 100 -vec ./samples/samples_ML.vec -bg negatives_ML.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 56 -h 70 
opencv_createsamples -img ./positive_images/MR.jpg -num 100 -vec ./samples/samples_MR.vec -bg negatives_MR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 45 -h 55 
opencv_createsamples -img ./positive_images/NL.jpg -num 100 -vec ./samples/samples_NL.vec -bg negatives_NL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 54 -h 73 
opencv_createsamples -img ./positive_images/NR.jpg -num 100 -vec ./samples/samples_NR.vec -bg negatives_NR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 56 
opencv_createsamples -img ./positive_images/OL.jpg -num 100 -vec ./samples/samples_OL.vec -bg negatives_OL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 40 -h 56 
opencv_createsamples -img ./positive_images/OR.jpg -num 100 -vec ./samples/samples_OR.vec -bg negatives_OR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 47 -h 72 
opencv_createsamples -img ./positive_images/PL.jpg -num 100 -vec ./samples/samples_PL.vec -bg negatives_PL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 54 -h 59 
opencv_createsamples -img ./positive_images/PR.jpg -num 100 -vec ./samples/samples_PR.vec -bg negatives_PR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 44 -h 53
opencv_createsamples -img ./positive_images/QL.jpg -num 100 -vec ./samples/samples_QL.vec -bg negatives_QL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 61 -h 55 
opencv_createsamples -img ./positive_images/QR.jpg -num 100 -vec ./samples/samples_QR.vec -bg negatives_QR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 54 -h 38 
opencv_createsamples -img ./positive_images/RL.jpg -num 100 -vec ./samples/samples_RL.vec -bg negatives_RL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 83 
opencv_createsamples -img ./positive_images/RR.jpg -num 100 -vec ./samples/samples_RR.vec -bg negatives_RR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 42 -h 81 
opencv_createsamples -img ./positive_images/SL.jpg -num 100 -vec ./samples/samples_SL.vec -bg negatives_SL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 43 -h 58 
opencv_createsamples -img ./positive_images/SR.jpg -num 100 -vec ./samples/samples_SR.vec -bg negatives_SR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 60 -h 70 
opencv_createsamples -img ./positive_images/TL.jpg -num 100 -vec ./samples/samples_TL.vec -bg negatives_TL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 70 -h 90 
opencv_createsamples -img ./positive_images/TR.jpg -num 100 -vec ./samples/samples_TR.vec -bg negatives_TR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 48 -h 62 
opencv_createsamples -img ./positive_images/UL.jpg -num 100 -vec ./samples/samples_UL.vec -bg negatives_UL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 93 
opencv_createsamples -img ./positive_images/UR.jpg -num 100 -vec ./samples/samples_UR.vec -bg negatives_UR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 42 -h 90 
opencv_createsamples -img ./positive_images/VL.jpg -num 100 -vec ./samples/samples_VL.vec -bg negatives_VL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 45 -h 83 
opencv_createsamples -img ./positive_images/VR.jpg -num 100 -vec ./samples/samples_VR.vec -bg negatives_VR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 49 -h 91 
opencv_createsamples -img ./positive_images/WL.jpg -num 100 -vec ./samples/samples_WL.vec -bg negatives_WL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 73 
opencv_createsamples -img ./positive_images/WR.jpg -num 100 -vec ./samples/samples_WR.vec -bg negatives_WR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 57 -h 89 
opencv_createsamples -img ./positive_images/XL.jpg -num 100 -vec ./samples/samples_XL.vec -bg negatives_XL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 61 
opencv_createsamples -img ./positive_images/XR.jpg -num 100 -vec ./samples/samples_XR.vec -bg negatives_XR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 41 -h 59 
opencv_createsamples -img ./positive_images/YL.jpg -num 100 -vec ./samples/samples_YL.vec -bg negatives_YL.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 84 -h 71 
opencv_createsamples -img ./positive_images/YR.jpg -num 100 -vec ./samples/samples_YR.vec -bg negatives_YR.txt -maxxangle 0.6 -maxyangle 0 -maxzangle 0.3 -maxidev 100 -bgcolor 0 -bgthresh 0 -w 63 -h 54 
