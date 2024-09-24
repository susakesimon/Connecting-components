#include "component.h"
#include "cimage.h"
#include "bmplib.h"
#include <deque>
#include <iomanip>
#include <iostream>
#include <cmath>
// You shouldn't need other #include's - Ask permission before adding

using namespace std;

// TO DO: Complete this function
CImage::CImage(const char* bmp_filename)
{
    //  Note: readRGBBMP dynamically allocates a 3D array
    //    (i.e. array of pointers to pointers (1 per row/height) where each
    //    point to an array of pointers (1 per col/width) where each
    //    point to an array of 3 unsigned char (uint8_t) pixels [R,G,B values])

    // ================================================
    // TO DO: call readRGBBMP to initialize img_, h_, and w_;
    img_ = readRGBBMP(bmp_filename, h_, w_);



    // Leave this check
    if(img_ == NULL) {
        throw std::logic_error("Could not read input file");
    }

    // Set the background RGB color using the upper-left pixel
    for(int i=0; i < 3; i++) {
        bgColor_[i] = img_[0][0][i];
    }

    // ======== This value should work - do not alter it =======
    // RGB "distance" threshold to continue a BFS from neighboring pixels
    bfsBgrdThresh_ = 60;

    // ================================================
    // TO DO: Initialize the vector of vectors of labels to -1
		for(int i=0; i<h_; i++){
            // labels_[i]i.push_back();
            vector<int>temp;
            for(int j=0; j<w_; j++){
                temp.push_back(-1);
            }
            labels_.push_back(temp);
        }


    // ================================================
    // TO DO: Initialize any other data members
    bgColor_[0] = img_[0][0][0];
    bgColor_[1] = img_[0][0][1];
    bgColor_[2] = img_[0][0][2];
}

// TO DO: Complete this function
CImage::~CImage()
{
    // Add code here if necessary
		deallocateImage(img_);


}

// Complete - Do not alter
bool CImage::isCloseToBground(uint8_t p1[3], double within) {
    // Computes "RGB" (3D Cartesian distance)
    double dist = sqrt( pow(p1[0]-bgColor_[0],2) +
                        pow(p1[1]-bgColor_[1],2) +
                        pow(p1[2]-bgColor_[2],2) );
    return dist <= within;
}

// TO DO: Complete this function
size_t CImage::findComponents()
{    
	int labelcnt = 0;
    uint8_t color[3];
    for(int i =0; i <h_; i++){
        for(int j=0; j<w_; j++){
            color[0] = img_[i][j][0];
            color[1] = img_[i][j][1];
            color[2] = img_[i][j][2];
            if(!(isCloseToBground(color, bfsBgrdThresh_)) && labels_[i][j]==-1){
                components_.push_back(bfsComponent(i, j, labelcnt));
                labelcnt++;
            }
        }
    }
	return labelcnt;
}

// Complete - Do not alter
void CImage::printComponents() const
{
    cout << "Height and width of image: " << h_ << "," << w_ << endl;
    cout << setw(4) << "Ord" << setw(4) << "Lbl" << setw(6) << "ULRow" << setw(6) << "ULCol" << setw(4) << "Ht." << setw(4) << "Wi." << endl;
    for(size_t i = 0; i < components_.size(); i++) {
        const Component& c = components_[i];
        cout << setw(4) << i << setw(4) << c.label << setw(6) << c.ulNew.row << setw(6) << c.ulNew.col
             << setw(4) << c.height << setw(4) << c.width << endl;
    }

		// cout << components_.size() << endl;

}


// TODO: Complete this function
int CImage::getComponentIndex(int mylabel) const
{
	for(unsigned int i=0; i<components_.size(); i++){
		if(components_[i].label == mylabel){
			return i;
		}
	}
	return -1;
}


// Nearly complete - TO DO:
//   Add checks to ensure the new location still keeps
//   the entire component in the legal image boundaries
void CImage::translate(int mylabel, int nr, int nc)
{
    // Get the index of specified component
    int cid = getComponentIndex(mylabel);
    if(cid < 0) {
        return;
    }
    int h = components_[cid].height;
    int w = components_[cid].width;

    // ==========================================================
    // ADD CODE TO CHECK IF THE COMPONENT WILL STILL BE IN BOUNDS
    // IF NOT:  JUST RETURN.
		if(nr < 0 || nr+h > h_ || nc+w > w_ || nc < 0 ){
			return;
		}
    // ==========================================================

    // If we reach here we assume the component will still be in bounds
    // so we update its location.
    Location nl(nr, nc);
    components_[cid].ulNew = nl;
}

// TO DO: Complete this function
void CImage::forward(int mylabel, int delta)
{
    int cid = getComponentIndex(mylabel);
    if(cid < 0 || delta <= 0) {
        return;
    }
    // Add your code here
    Component temp = components_[cid];
    unsigned int size = cid+delta;
    if(size >= components_.size()){
        size = components_.size()-1;
    }
    for(unsigned int i=cid; i<size; i++){
        components_[i] = components_[i+1];
    }
    components_[size] = temp;
}

// TO DO: Complete this function
void CImage::backward(int mylabel, int delta)
{
    int cid = getComponentIndex(mylabel);
    if(cid < 0 || delta <= 0) {
        return;
    }
    // Add your code here
    Component temp = components_[cid];
    int size = cid-delta;
    if(size <0 ){
        size = 0;
    }
    for(int i=cid; i>size; i--){
        components_[i] = components_[i-1];
    }
    components_[size] = temp;
}

// TODO: complete this function
void CImage::save(const char* filename)
{
    // Create another image filled in with the background color
    uint8_t*** out = newImage(bgColor_);

    // Add your code here
    for(unsigned int w=0; w<components_.size(); w++){
        int variableRow = components_[w].ulOrig.row;
        int variableCol = components_[w].ulOrig.col;
        int newVariableRow = components_[w].ulNew.row;
        int newVariableCol = components_[w].ulNew.col;
        for(int i=0; i<components_[w].height; i++){
			for(int j=0; j<components_[w].width; j++){
                if (labels_[variableRow+i][variableCol+j] == components_[w].label){
                    out[newVariableRow+i][newVariableCol+j][0] = 
                    img_[variableRow+i][variableCol+j][0];

                    out[newVariableRow+i][newVariableCol+j][1] = 
                    img_[variableRow+i][variableCol+j][1];

                    out[newVariableRow+i][newVariableCol+j][2] = 
                    img_[variableRow+i][variableCol+j][2];
                }
				
			}
		}
    }
    writeRGBBMP(filename, out, h_, w_);
    // Add any other code you need after this
    deallocateImage(out);
    
}

// Complete - Do not alter - Creates a blank image with the background color
uint8_t*** CImage::newImage(uint8_t bground[3]) const
{
    uint8_t*** img = new uint8_t**[h_];
    for(int r=0; r < h_; r++) {
        img[r] = new uint8_t*[w_];
        for(int c = 0; c < w_; c++) {
            img[r][c] = new uint8_t[3];
            img[r][c][0] = bground[0];
            img[r][c][1] = bground[1];
            img[r][c][2] = bground[2];
        }
    }
    return img;
}

// To be completed
void CImage::deallocateImage(uint8_t*** img) const
{
    // Add your code here
	// if(!(img_ == NULL)){
        for(int r=0; r < h_; r++) {
            for(int c = 0; c < w_; c++) {
                // for(int h=0; h<3; h++){
                //     delete img[r][c][h];
                // }
            delete [] img[r][c];
            }
          delete [] img[r];
        }
        delete [] img;
	// 		img_ = NULL;
	// }

}

// TODO: Complete the following function or delete this if
//       you do not wish to use it.
Component CImage::bfsComponent(int pr, int pc, int mylabel)
{
    // Arrays to help produce neighbors easily in a loop
    // by encoding the **change** to the current location.
    // Goes in order N, NW, W, SW, S, SE, E, NE
    int neighbor_row[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int neighbor_col[8] = {0, -1, -1, -1, 0, 1, 1, 1};
    // bfs stands for breathe first search
    // the wave thing
    deque<Location> queue;
		// to make sure i get in while loop
		Location loc(pr, pc);
		queue.push_front(loc);
		Location start(pr, pc);

		int minRow = h_;
		int maxRow = 0;
		int minCol = w_;
		int maxCol = 0;

		while(!(queue.empty())){
			loc = queue.front();
			queue.pop_front();
			for(int i=0; i<8; i++){
				// img_[pr + neighbor_row[i]][pc + neighbor_col[i]];
				otherrow = loc.row + neighbor_row[i];
				othercol = loc.col + neighbor_col[i];
				Location temp(otherrow, othercol);
				if(otherrow >= 0 && otherrow < h_ && othercol >=0 && othercol < w_){
					if(!(isCloseToBground(img_[otherrow][othercol], bfsBgrdThresh_)) && labels_[otherrow][othercol] == -1 ){
						labels_[otherrow][othercol] = mylabel;
						queue.push_back(temp);
						if(maxRow < otherrow){
							maxRow = otherrow;
						}
						if(minRow > otherrow){
							minRow = otherrow;
						}
						if(maxCol < othercol){
							maxCol = othercol;
						}
						if(minCol > othercol){
							minCol = othercol;
						}
					}
				}
			}
		}
    Component finish(Location(minRow, minCol), maxRow-minRow+1, maxCol-minCol+1, mylabel);
    return finish;
}

// Complete - Debugging function to save a new image
void CImage::labelToRGB(const char* filename)
{
    //multiple ways to do this -- this is one way
    vector<uint8_t[3]> colors(components_.size());
    for(unsigned int i=0; i<components_.size(); i++) {
        colors[i][0] = rand() % 256;
        colors[i][1] = rand() % 256;
        colors[i][2] = rand() % 256;
    }

    for(int i=0; i<h_; i++) {
        for(int j=0; j<w_; j++) {
            int mylabel = labels_[i][j];
            if(mylabel >= 0) {
                img_[i][j][0] =  colors[mylabel][0];
                img_[i][j][1] =  colors[mylabel][1];
                img_[i][j][2] =  colors[mylabel][2];
            } else {
                img_[i][j][0] = 0;
                img_[i][j][1] = 0;
                img_[i][j][2] = 0;
            }
        }
    }
    writeRGBBMP(filename, img_, h_, w_);
}

// Complete - Do not alter
const Component& CImage::getComponent(size_t i) const
{
    if(i >= components_.size()) {
        throw std::out_of_range("Index to getComponent is out of range");
    }
    return components_[i];
}

// Complete - Do not alter
size_t CImage::numComponents() const
{
    return components_.size();
}

// Complete - Do not alter
void CImage::drawBoundingBoxesAndSave(const char* filename)
{
    for(size_t i=0; i < components_.size(); i++){
        Location ul = components_[i].ulOrig;
        int h = components_[i].height;
        int w = components_[i].width;
        for(int i = ul.row; i < ul.row + h; i++){
            for(int k = 0; k < 3; k++){
                img_[i][ul.col][k] = 255-bgColor_[k];
                img_[i][ul.col+w-1][k] = 255-bgColor_[k];

            }
            // cout << "bb " << i << " " << ul.col << " and " << i << " " << ul.col+w-1 << endl; 
        }
        for(int j = ul.col; j < ul.col + w ; j++){
            for(int k = 0; k < 3; k++){
                img_[ul.row][j][k] = 255-bgColor_[k];
                img_[ul.row+h-1][j][k] = 255-bgColor_[k];

            }
            // cout << "bb2 " << ul.row << " " << j << " and " << ul.row+h-1 << " " << j << endl; 
        }
    }
    writeRGBBMP(filename, img_, h_, w_);
}

// Add other (helper) function definitions here


