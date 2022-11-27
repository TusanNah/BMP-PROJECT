#include "iostream"
#include "fstream"
using namespace std;

struct PIXEL_8bits {
	unsigned char B;
};
struct PIXEL_24bits { 
	unsigned char B;
	unsigned char G;
	unsigned char R;
};
struct PIXEL_32bits {
	unsigned char A;
	unsigned char B;
	unsigned char G;
	unsigned char R;
};
struct COLORTABLE { //Bang mau danh cho anh 8 bits
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char reserved;
};
#pragma pack(1)
struct HEADER {
	char Singature[2];
	long FileSize; //Kich thuoc File
	long Reserved; //Phan danh rieng
	long DataOffset; //Dia chi phan bat dau luu du lieu diem anh
	
};
struct DIB {
	
	long DIB_Size;
	long Width; //So pixel theo chieu rong
	long Height; //So pixel theo chieu dai
	
	short Planes; //So lop mau (=1)
	short Bpp; //Bits per pixel (1, 4, 8 ,16, 24, 32)
	
	long Compression; //Cach nen anh
	
	long ImageSize; //Kich thuoc phan du lieu diem anh
	
	long XpixelsPerM; //Do phan giai theo phuong ngang
	long YpixelsPerM; //Do phan giai theo phuong dung
	
	long ColorsUsed; //So mau trong bang mau
	long ImportantColor; //So mau quan trong
};
struct BMP {
	HEADER header;
	DIB dib;
	char* pDIB_Reserved; // con tro toi phan du cua DIB	
	COLORTABLE colortable[256]; //Tao bang mau neu anh la 8 bit
	char* pImageData;
};
void Intro() {
	cout << endl << "----------BMP PROJECT----------";
	cout << endl << "Ho va ten : Truong Anh Tuan.";
	cout << endl << "Ma so: 21120589.";
	cout << endl << "Lop: 21CTT5"<<endl;
}
int readFile(const char* filename, BMP& bmp) {
	ifstream fin(filename, ios::binary);
	if (!fin)
		return 0;
	fin.read((char*)&bmp.header, 14);
	fin.read((char*)&bmp.dib, 40);
	if (bmp.dib.DIB_Size > 40) { //Vi du Dib_Size >40 
		int remain = bmp.dib.DIB_Size - 40; //kich thuoc phan du trong DIB 
		bmp.pDIB_Reserved = new char[remain];
		fin.read(bmp.pDIB_Reserved, remain);
	}
	if (bmp.dib.Bpp == 8) {
		fin.read((char*)&bmp.colortable, sizeof(bmp.colortable)); //Neu la anh 8 bit thi phai doc bang mau
	}
	bmp.pImageData = new char[bmp.dib.ImageSize]; //cap phat vung nho cho Pixel Data
	fin.read(bmp.pImageData, bmp.dib.ImageSize);
	fin.close();
	return 1;
}
void ShowInfo(BMP bmp) {
	cout << endl << "Signature: " << bmp.header.Singature << endl;
	cout << "FileSize: " << bmp.header.FileSize << endl;
	cout << "Reserved: " << bmp.header.Reserved << endl;
	cout << "DataOffset: " << bmp.header.DataOffset << endl;
	cout << "DIB_Size: " << bmp.dib.DIB_Size << endl;
	cout << "Width: " << bmp.dib.Width << endl;
	cout << "Height: " << bmp.dib.Height << endl;
	cout << "Planes: " << bmp.dib.Planes << endl;
	cout << "Bits per pixel: " << bmp.dib.Bpp << endl;
	cout << "Compression: " << bmp.dib.Compression << endl;
	cout << "ImageSize: " << bmp.dib.ImageSize << endl;
	cout << "XpixelsPerM: " << bmp.dib.XpixelsPerM << endl;
	cout << "YpixelsPerM: " << bmp.dib.YpixelsPerM << endl;
	cout << "ColorsUsed: " << bmp.dib.ColorsUsed << endl;
	cout << "ImportantColor: " << bmp.dib.ImportantColor << endl;
}
int writeFile(const char* filename, BMP bmp) {
	ofstream fout(filename, ios::binary);
	if (!fout)
		return 0;
	cout << endl << "Mo file output thanh cong" << endl;
	fout.write((char*)&bmp.header, 14);
	fout.write((char*)&bmp.dib, 40);
	if (bmp.dib.DIB_Size > 40)
		fout.write(bmp.pDIB_Reserved, bmp.dib.DIB_Size - 40);
	if (bmp.dib.Bpp == 8) {
		fout.write((char*)&bmp.colortable, sizeof(bmp.colortable)); //Neu la anh 8 bit thi phai ghi bang mau
	}
	fout.write(bmp.pImageData, bmp.dib.ImageSize);	
	fout.close();
	return 1;
}
int ConvertTo8bit(BMP srcBMP, BMP &dstBMP) {
	if ((srcBMP.dib.Bpp != 24 && srcBMP.dib.Bpp != 32) || srcBMP.pImageData == NULL)
		return 0;
	cout << endl << "Dang thuc hien giai nen anh sang 8 bit " << endl;
	dstBMP.dib = srcBMP.dib;
	dstBMP.header = srcBMP.header;
	if (dstBMP.dib.DIB_Size > 40) {
		int remain = dstBMP.dib.DIB_Size - 40;
		dstBMP.pDIB_Reserved = new char[remain];
	}
	dstBMP.dib.Compression = 0; //Dam bao anh dst phai co Compression = 0
	dstBMP.dib.Bpp = 8;
	// Tinh padding byte cho Source
	int dstWidthInBytes = dstBMP.dib.Width * dstBMP.dib.Bpp / 8; 
	int dst_paddingBytes = (4 - (dstWidthInBytes) % 4) % 4;
	// Tinh padding byte cho Destination
	int srcWidthInBytes = srcBMP.dib.Width * srcBMP.dib.Bpp / 8;
	int src_paddingBytes = (4 - (srcWidthInBytes) % 4) % 4;
	
	//          ImageSize=	 Width*Height*BitsPerPixel/8	+	PaddingBytes*Height
	dstBMP.dib.ImageSize = dstBMP.dib.Width * dstBMP.dib.Height * dstBMP.dib.Bpp / 8 + dst_paddingBytes * dstBMP.dib.Height;
	//			Khoi tao con tro ImageData
	dstBMP.pImageData = new char[dstBMP.dib.ImageSize];
	// Con tro cua tung` dong
	char* pSrcRow = srcBMP.pImageData;
	char* pDstRow = dstBMP.pImageData;
	// Di qua tung diem pixel tren 1 dong`
	int srcDOTP = srcBMP.dib.Bpp / 8; //Khoang cach giua 2 pixels trong file Source (Distance Of Two Pixels) tinh bang byte
	
	int dstDOTP = dstBMP.dib.Bpp / 8; //Khoang cach giua 2 pixels trong file Dst (Distance Of Two Pixels) tinh bang byte
	
	int srcDOTR = srcBMP.dib.Width * srcDOTP + src_paddingBytes;	//Khoang cach giua 2 dong trong file Source (Distance Of TWo Rows)
	
	int dstDOTR = dstBMP.dib.Width * dstDOTP + dst_paddingBytes;	//Khoang cach giua 2 dong trong file Dst (Distance Of Two Rows)
	for (int i = 0; i < 256; i++) { //Tao bang mau cho anh 8 bit
		dstBMP.colortable[i].B = i;
		dstBMP.colortable[i].G = i;
		dstBMP.colortable[i].R = i;
		dstBMP.colortable[i].reserved = 0;
	}
	for (int i = 0; i < dstBMP.dib.Height; i++) {
		char* temp1 = pSrcRow; //Con tro dau` dong` thu i cua Source
		char* temp2 = pDstRow; //Con tro dau` dong` thu i cua Destination
		for (int j = 0; j < dstBMP.dib.Width; j++) {
			unsigned char B, G, R, avg;
			if (srcBMP.dib.Bpp == 24) {
				B = temp1[0];
				G = temp1[1];
				R = temp1[2];
			}
			else if (srcBMP.dib.Bpp == 32) {
				B = temp1[1];
				G = temp1[2];
				R = temp1[3];
			}
			avg = (char)((B + R + G) / 3);
			temp2[0] = avg;
			// Tinh trung binh cua 3 bytes trong Source
			temp1 += srcDOTP;
			temp2 += dstDOTP;
		}
		pSrcRow += srcDOTR; //Sau moi lan` lap. phai dich chuyen con tro sang dong` tiep theo trong Source
		pDstRow += dstDOTR; //Sau moi lan` lap. phai dich chuyen con tro sang dong`  tiep theo trong Destination
	}
	return 1;
}
// Ham tinh gia tri trung diem anh cho 8 bits
void Average8(PIXEL_8bits& p, char* pRow, int srcDOTR, int srcDOTP, int height, int width) {
	unsigned int bgr = 0;
	// bgr[0] giu gia tri mau blue
	p.B = 0;
	for (int i = 0; i < height; i++) {
		char* temp = pRow;
		for (int j = 0; j < width; j++) {
			bgr += (unsigned char)temp[0];
			temp += srcDOTP;
		}
		pRow += srcDOTR;
	}
	bgr = (unsigned char)(bgr / (height * width));
	p.B = bgr;
}
// Ham tinh gia tri trung diem anh cho 24 bits
void Average24(PIXEL_24bits& p, char* pRow, int srcDOTR, int srcDOTP, int height, int width) { 
	unsigned int bgr[3];
	bgr[0] = bgr[1] = bgr[2] = 0;
	// bgr[0] giu gia tri mau blue, bgr[1] giu gia tri mau green, bgr[2] giu gia tri mau red.
	p.B = p.R = p.G = 0;
	for (int i = 0; i < height; i++) {
		char* temp = pRow;
		for (int j = 0; j < width; j++) {
			bgr[0] += (unsigned char)temp[0];
			bgr[1] += (unsigned char)temp[1];
			bgr[2] += (unsigned char)temp[2];
			temp += srcDOTP;
		}
		pRow += srcDOTR;
	}
	for (int i = 0; i < 3; i++) {
		bgr[i] = (unsigned char)(bgr[i] / (height * width));
	}
	p.B = bgr[0];
	p.G = bgr[1];
	p.R = bgr[2];
}
// Ham tinh gia tri trung diem anh cho 32 bits
void Average32(PIXEL_32bits& p, char* pRow, int srcDOTR, int srcDOTP, int height, int width) {
	unsigned int bgr[4]; // Để tránh trường hợp tổng giá trị mỗi màu vượt quá giá trị cho phép của unsigned char là 255, ta tạm dùng biến unsigned int để lưu tổng giá trị mỗi màu.
	bgr[0] = bgr[1] = bgr[2] = bgr[3] = 0;
	//bgr[0] giu gia tri mau A, bgr[1] giu gia tri mau blue, bgr[2] giu gia tri mau green, bgr[3] giu gia tri mau red.
	p.A=p.B = p.R = p.G = 0;
	for (int i = 0; i < height; i++) {
		char* temp = pRow; // pRow là con trỏ trỏ vào byte thứ nhất của Pixel đầu tiên trong ô kích thước Height*Width.
		for (int j = 0; j < width; j++) {
			bgr[0] +=(unsigned char)temp[0]; // Tính tổng giá trị A trong ô có kích thước là Height*Width (vì mảng bgr[] có kiểu là unsigned int nên khi cộng ta phải ép kiểu).
			bgr[1] +=(unsigned char)temp[1]; // Tính tổng giá trị B trong ô có kích thước là Height*Width (vì mảng bgr[] có kiểu là unsigned int nên khi cộng ta phải ép kiểu).	
			bgr[2] +=(unsigned char)temp[2]; // Tính tổng giá trị G trong ô có kích thước là Height*Width (vì mảng bgr[] có kiểu là unsigned int nên khi cộng ta phải ép kiểu).	
			bgr[3] +=(unsigned char)temp[3]; // Tính tổng giá trị R trong ô có kích thước là Height*Width (vì mảng bgr[] có kiểu là unsigned int nên khi cộng ta phải ép kiểu).	
			temp += srcDOTP; // Di chuyển con trỏ sang Pixel tiếp theo
		}
		pRow += srcDOTR; // Di chuyển con trỏ xuống dòng mới
	}
	for (int i = 0; i < 4; i++) {
		bgr[i] = (unsigned char)(bgr[i] / (height * width)); // Chia tổng cho Height*Width để được giá trị trung bình của điểm ảnh để gán vào Pixel của ảnh Destination
	}
	p.A = bgr[0]; // Gán các giá trị bgr[0] vào lại p.A 
	p.B = bgr[1]; // Gán các giá trị bgr[1] vào lại p.B (blue) .
	p.G = bgr[2]; // Gán các giá trị bgr[0] vào lại p.G (green).
	p.R = bgr[3]; // Gán các giá trị bgr[0] vào lại p.R (red).
}
void ZoomBMP(BMP srcBMP, BMP& dstBMP,  int& S) {
	PIXEL_24bits p24; // Giá trị trung bình các điểm ảnh của hàm 24 bits
	PIXEL_32bits p32; // Giá trị trung bình các điểm ảnh của hàm 32 bits
	PIXEL_8bits p8; // Giá trị trung bình các điểm ảnh của hàm 8 bits
	dstBMP.dib = srcBMP.dib;
	dstBMP.header = srcBMP.header;
	if (dstBMP.dib.DIB_Size > 40) {
		int remain = dstBMP.dib.DIB_Size - 40;
		dstBMP.pDIB_Reserved = new char[remain];
	}	
	
	dstBMP.dib.Height /= S; // Chiều cao của ảnh Dest 
	dstBMP.dib.Width /= S;  // Chiều ngang của ảnh Dest
	//if (srcBMP.dib.Height % S != 0)
	//	dstBMP.dib.Height += 1;
	//if (srcBMP.dib.Width % S != 0)
	//	dstBMP.dib.Width += 1;

	// Tính Padding byte
	int dstWidthInBytes = dstBMP.dib.Width * dstBMP.dib.Bpp / 8; 
	int dst_paddingBytes = (4 - (dstWidthInBytes) % 4) % 4;

	int srcWidthInBytes = srcBMP.dib.Width * srcBMP.dib.Bpp / 8;
	int src_paddingBytes = (4 - (srcWidthInBytes) % 4) % 4;
	
	dstBMP.dib.Compression = 0; //Dam bao anh dst phai co Compression = 0

	dstBMP.dib.ImageSize = dstBMP.dib.Width * dstBMP.dib.Height * dstBMP.dib.Bpp / 8 + dst_paddingBytes * dstBMP.dib.Height;
	dstBMP.pImageData = new char[dstBMP.dib.ImageSize];
	if (S == 1 || S <= 0) { // Neu S=1 thi ko can phai thu nho
		dstBMP.pImageData = srcBMP.pImageData;
		return;
	}
	
	// Tương tự như hàm zoom, tính DOTP, DOTR của 2 ảnh ...
	int srcDOTP = srcBMP.dib.Bpp / 8;

	int srcDOTR = srcBMP.dib.Width * srcDOTP + src_paddingBytes;

	int dstDOTP = dstBMP.dib.Bpp / 8;

	int dstDOTR = dstBMP.dib.Width * dstDOTP + dst_paddingBytes;
	if (dstBMP.dib.Bpp == 8) { // Nếu là ảnh 8 bits thì ghi bảng màu cho ảnh
		for (int i = 0; i < 256; i++) {
			dstBMP.colortable[i].B = i;
			dstBMP.colortable[i].G = i;
			dstBMP.colortable[i].R = i;
			dstBMP.colortable[i].reserved = 0;
		}
	}
	// Cách di chuyển con trỏ lấy ý tưởng từ từ hàm ConvertTo8bits
	char* pSrcSquare = srcBMP.pImageData;
	char* pDstRow = dstBMP.pImageData;
	for (int i = 0; i < srcBMP.dib.Height / S; i++) {
		char* temp1 = pSrcSquare;
		char* temp2 = pDstRow;
		for (int j = 0; j < srcBMP.dib.Width / S; j++) {
			if (dstBMP.dib.Bpp == 32) { 
				Average32(p32,temp1, srcDOTR, srcDOTP, S, S);
				temp2[0] = p32.A; // Gán giá trị trung bình A tính được trong 1 ô có kích thước là Height*Width (cụ thể là S*S).
				temp2[1] = p32.B; // Gán giá trị trung bình B tính được trong 1 ô có kích thước là Height*Width (cụ thể là S*S).
				temp2[2] = p32.G; // Gán giá trị trung bình G tính được trong 1 ô có kích thước là Height*Width (cụ thể là S*S).
				temp2[3] = p32.R; // Gán giá trị trung bình R tính được trong 1 ô có kích thước là Height*Width (cụ thể là S*S).
			
			}
			else if (dstBMP.dib.Bpp == 24) {
				Average24(p24, temp1, srcDOTR, srcDOTP, S, S);
				temp2[0] = p24.B; // Tương tự như hàm 32 bits nhưng ko có giá trị A.
				temp2[1] = p24.G;
				temp2[2] = p24.R;
			}
			else {
				Average8(p8, temp1, srcDOTR, srcDOTP, S, S);
				temp2[0] = p8.B; // Tương tự như hàm 32 bits nhưng chỉ có mỗi giá trị B.
			}
			temp1 += srcDOTP * S; /* Trong ảnh Source, thay vì nhảy qua 1 pixel(theo chiều ngang) như hàm ConvertTo8bits
									thì ta phải nhảy S pixels (vì phải nhảy qua 1 hình vuông có chiều ngang là S). */
			temp2 += dstDOTP; // Trong ảnh Destination thì tương tự hàm ConvertTo8bits 
		}
		pSrcSquare += srcDOTR * S; /*Thay vì nhảy xuống 1 dòng(theo chiều dọc) như hàm ConvertTo8bits 
								   thì ta phải nhảy S dòng(vì phải nhảy xuống 1 hình vuông có chiều dọc là S) */
		pDstRow += dstDOTR; // Trong ảnh Destination thì tương tự như hàm ConvertTo8bits
	}
	// Tính các pixel thừa
	/*if (srcBMP.dib.Width % S != 0) {
		cout << endl << "Du chieu rong";
		pSrcSquare = srcBMP.pImageData + srcDOTP * S* (srcBMP.dib.Width/S);
		pDstRow = dstBMP.pImageData + dstDOTP * (srcBMP.dib.Width/S);
		for (int i = 0; i < srcBMP.dib.Height / S; i++) {
			char* temp1 = pSrcSquare;
			char* temp2 = pDstRow;
			int height = S;
			int width = srcBMP.dib.Width % S;
			if (dstBMP.dib.Bpp == 32) {
				Average32(p32, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p32.A;
				temp2[1] = p32.B;
				temp2[2] = p32.G;
				temp2[3] = p32.R;

			}
			else if (dstBMP.dib.Bpp == 24) {
				Average24(p24, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p24.B;
				temp2[1] = p24.G;
				temp2[2] = p24.R;
			}
			else {
				Average8(p8, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p8.B;
			}
			temp1 += srcDOTP * S;
			temp2 += dstDOTP;
		}
	}
	if (srcBMP.dib.Height % S != 0) {
		cout << endl << "Du chieu cao: ";
		pSrcSquare = srcBMP.pImageData + srcDOTR*(srcBMP.dib.Width / S);
		pDstRow = dstBMP.pImageData + dstDOTR * (srcBMP.dib.Height/S);
		char* temp1 = pSrcSquare;
		char* temp2 = pDstRow;
		for (int i = 0; i < srcBMP.dib.Width / S; i++) {
			int height = srcBMP.dib.Height % S;
			int width = S;
			if (dstBMP.dib.Bpp == 32) {
				Average32(p32, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p32.A;
				temp2[1] = p32.B;
				temp2[2] = p32.G;
				temp2[3] = p32.R;

			}
			else if (dstBMP.dib.Bpp==24) {
				Average24(p24, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p24.B;
				temp2[1] = p24.G;
				temp2[2] = p24.R;
			}
			else {
				Average8(p8, temp1, srcDOTR, srcDOTP, height, width);
				temp2[0] = p8.B;
			}
			temp1 += srcDOTP * S;
			temp2 += dstDOTP;
		}
	}
	if (srcBMP.dib.Height % S != 0 && srcBMP.dib.Width % S != 0) {
		cout << "Du chieu cao va chieu rong ";
		char* temp1 = srcBMP.pImageData + srcDOTR *S*(srcBMP.dib.Height/S) + srcDOTP *S* (srcBMP.dib.Width/S);
		char* temp2=dstBMP.pImageData+ dstDOTR * (srcBMP.dib.Height /S)+ dstDOTP * (srcBMP.dib.Width/S);
		int height = srcBMP.dib.Height % S;
		cout << endl << height;
		int width = srcBMP.dib.Width % S;
		if (dstBMP.dib.Bpp == 32) {
			Average32(p32, temp1, srcDOTR, srcDOTP, height, width);
			temp2[0] = p32.A;
			temp2[1] = p32.B;
			temp2[2] = p32.G;
			temp2[3] = p32.R;

			}
		else if (dstBMP.dib.Bpp == 24) {
			Average24(p24, temp1, srcDOTR, srcDOTP, height, width);
			temp2[0] = p24.B;
			temp2[1] = p24.G;
			temp2[2] = p24.R;
			}
		else {
			Average8(p8, temp1, srcDOTR, srcDOTP, height, width);
			temp2[0] = p8.B;
		}
	}*/
}

void ReleaseMemory(BMP& src, BMP& dstBMP1, BMP& dstBMP2) { //Giai phóng bộ nhớ
	delete[]src.pImageData, dstBMP1.pImageData, dstBMP2.pImageData;
	src.pImageData = dstBMP1.pImageData = dstBMP2.pImageData = NULL;
}

int main(int argc, char* argv[]) {
	BMP srcBMP;
	BMP convertBMP;
	BMP zoomBMP;
	Intro();
	if (argc < 4 || argc >5) { // Ta chỉ có tối đa được 5 tham số và ít nhất phải là 4 tham số 
		// Ví dụ nếu muốn convert ảnh thì phải truyền 21120589.exe -conv E:\input.bmp E:\output.bmp
		// Còn muôn zoom ảnh thì phải truyền 21120589.exe -zoom E:\input.bmp E:\output.bmp 3 (3 là tỉ lệ S)
		cout << endl << "Nhap lai tham so dong lenh.";
		return 1;
	}
	else if (argc == 4) { // Nếu số lượng tham số là 4 thì ta đang thực hiện hàm conv
		if (_strcmpi(argv[1], "-conv") != 0) { // Kiểm tra xem có nhập đúng tham số không
			cout << endl << "Nhap sai tham so dong lenh.";
			return 1;
		}
		else if (_strcmpi(argv[1], "-conv") == 0) {
			readFile(argv[2], srcBMP); // argv[2] là đường dẫn file input
			cout << endl << "--------Thong tin anh goc--------";
			ShowInfo(srcBMP);
			ConvertTo8bit(srcBMP, convertBMP);
			cout << endl << "------Thong tin anh 8 bits------";
			ShowInfo(convertBMP); 
			cout << endl << "Chuyen sang anh 8 bits thanh cong.";
			writeFile(argv[3], convertBMP); // argv[3] là đường dẫn file output
		}
	}
	else if (argc == 5) { // Nếu số lượng tham số là 5 thì ta đang thực hàm zoom
		if (_strcmpi(argv[1], "-zoom") != 0 || atoi(argv[4])<=0) { // Kiểm tra xem có nhập đúng tham số không
			cout << endl << "Nhap sai tham so dong lenh.";
			return 1;
		}
		else if (_strcmpi(argv[1], "-zoom") == 0 && atoi(argv[4])>0) {
			cout << endl << "Da nhap dung";
			int S = atoi(argv[4]);
			readFile(argv[2], srcBMP); // argv[2] là đường dẫn file input
			cout << endl << "--------Thong tin anh goc--------";
			ShowInfo(srcBMP);
			ZoomBMP(srcBMP, zoomBMP, S);
			cout << endl << "-------Thong tin anh Zoom-------";
			ShowInfo(zoomBMP);
			cout << endl << "Zoom anh thanh cong.";
			writeFile(argv[3], zoomBMP); // argv[3] là đường dẫn file output
		}
	}
	ReleaseMemory(srcBMP, convertBMP, zoomBMP); // Giải phóng các con trỏ
	return 1;
}