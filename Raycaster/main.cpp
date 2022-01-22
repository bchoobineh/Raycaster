
// Input: 2D array of integers represent colour
//          i, j, new Color(some value)


// Color(i,j) = x
// Contiguous Region 
// No diagonals

void recursiveHelper(int i, int j, std::vector<std::vector<Color>>& data, std::vector<std::vector<bool>> visited, Color updatedColor, Color original)
{
	if (i < 0 || j < 0 || i >= data.size(); || j >= data[0].size())
		return;

	visited[i][j] = true;

	if (data[i][j] == original)
	{
		// Updating the color
		data[i][j] = updatedColor;

		recursiveHelper(i + 1, j, data, visited, updatedColor, original);
		recursiveHelper(i - 1, j, data, visited, updatedColor, original);
		recursiveHelper(i, j+1, data, visited, updatedColor, original);
		recursiveHelper(i, j-1, data, visited, updatedColor, original);

	}
}

void fillAlgorithm(int i, int j, Color color, std::vector<std::vector<Color>> data)
{
	if (i < 0 || j < 0 || i >= data.size(); || j >= data[0].size())
		return;

	std::vector<std::vector<bool>> visited;
	for (int i = 0; i < data.size(); i++)
	{
		std::vector<bool> row(data[0].size(), false);
	}






}
