using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

class Program
{
    static async Task Main(string[] args)
    {
        if (args.Length < 1)
        {
            Console.WriteLine("File path not specified");
            return;
        }

        string filePath = args[0];

        if (!File.Exists(filePath))
        {
            Console.WriteLine("File not found. Please, check the path and try again.");
            return;
        }

        Console.WriteLine("Enter characters without spaces: ");
        string charactersToRemove = Console.ReadLine();

        string fileContent = await File.ReadAllTextAsync(filePath);

        string modifiedContent = RemoveCharacters(fileContent, charactersToRemove);

        await File.WriteAllTextAsync(filePath, modifiedContent);

        Console.WriteLine("Success");
    }

    static string RemoveCharacters(string input, string charactersToRemove)
    {
        var charsToRemoveSet = new HashSet<char>(charactersToRemove);

        return new string(input.Where(c => !charsToRemoveSet.Contains(c)).ToArray());
    }
}