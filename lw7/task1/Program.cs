using System;
using System.IO;
using System.Threading.Tasks;

namespace task1
{
    class Program
    {
        static async Task Main(string[] args)
        {
            Console.WriteLine("Введите путь к файлу:");
            string filePath = Console.ReadLine();

            Console.WriteLine("Введите список символов для удаления (без пробелов):");
            string charactersToRemove = Console.ReadLine();

            string text = await File.ReadAllTextAsync(filePath);

            foreach (char c in charactersToRemove)
            {
                text = text.Replace(c.ToString(), "");
            }

            await File.WriteAllTextAsync(filePath, text);
            Console.WriteLine("Изменения сохранены.");
        }
    }
}