class ImgMask{
	public:
		int **imgMask;
		int row;
		int col;
		ImgMask(int r, int c){
			row = r;
			col = c;
			imgMask = new int *[row];
			for(int i=0;i<row;i++)
				imgMask[i] = new int[col];
		}
		
};