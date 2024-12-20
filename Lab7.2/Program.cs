using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Net.Http;
using System.Reflection;
using System.Text.Json.Nodes;
using System.Threading.Tasks;

class Program
{
    private static readonly HttpClient httpClient = new HttpClient();
    private const string apiUrl = "https://dog.ceo/api/breeds/image/random";
    private const string downloadFolder = "images";

    static async Task Main(string[] args)
    {
        int numberOfRequests = 10;

        Directory.CreateDirectory(downloadFolder);

        // Синхронные запросы
        Console.WriteLine("Sync requests:");
        Stopwatch stopwatch = Stopwatch.StartNew();
        for (int i = 0; i < numberOfRequests; i++)
        {
            await FetchDogImageAsync(i);
        }
        stopwatch.Stop();
        Console.WriteLine($"Time: {stopwatch.ElapsedMilliseconds} ms\n");

        // Параллельные запросы
        Console.WriteLine("Async requests:");
        stopwatch.Restart();
        var tasks = new List<Task>();
        for (int i = 0; i < numberOfRequests; i++)
        {
            tasks.Add(FetchDogImageAsync(i));
        }
        await Task.WhenAll(tasks);
        stopwatch.Stop();
        Console.WriteLine($"Time: {stopwatch.ElapsedMilliseconds} ms");
    }

    private static async Task FetchDogImageAsync(int index)
    {
        try
        {
            var response = await httpClient.GetAsync(apiUrl);
            response.EnsureSuccessStatusCode();

            var jsonString = await response.Content.ReadAsStringAsync();
            var jsonObject = JsonObject.Parse(jsonString);
            var imageUrl = jsonObject["message"].ToString();
            Console.WriteLine($"Starting download from url: {imageUrl}");
            await DownloadImageAsync(imageUrl, index);
        }
        catch (HttpRequestException e)
        {
            Console.WriteLine($"Failed to start download. Error: {e.Message}");
        }
    }

    private static async Task DownloadImageAsync(string imageUrl, int index)
    {
        try
        {
            var imageResponse = await httpClient.GetAsync(imageUrl);
            imageResponse.EnsureSuccessStatusCode();

            var imagePath = Path.Combine(downloadFolder, $"dog_{index}.jpg");
            await using var fs = new FileStream(imagePath, FileMode.Create, FileAccess.Write, FileShare.None);
            await imageResponse.Content.CopyToAsync(fs);

            Console.WriteLine($"Image from url: {imageUrl} dowload successfully");
        }
        catch (Exception e)
        {
            Console.WriteLine($"Failed to load image. Error: {e.Message}");
        }
    }
}