using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.IO.Compression;
//using UnityL10nToolCppCLI;

namespace UnityL10nToolPatcherCShop
{
    class OnlineUpdateCLI
    {
        public enum SelectedEnum
        {
            None = 0,
            Manual = 1,
            GitHub = 2,
        }
        public SelectedEnum Selected;
        public string currentVersion;
        public string currentVersionURL;
        public string manualZipURL;
        public string gitHubOwner;
        public string gitHubRepo;
        public string _gitHubDownloadURL;
    }
    /// <summary>
    /// CustomSplash.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class CustomSplash : Window
    {
        public JObject patcherJson;
        public CustomSplash()
        {
            InitializeComponent();
        }

        private async void WailtFor2Second()
        {
            await Task.Delay(2000);
        }

        private string lastestVersionStr;
        private async Task<bool> CheckIsLastestVersionInOnline(OnlineUpdateCLI onlineUpdate)
        {
            WebClient webClient = new WebClient();
            // https://stackoverflow.com/questions/47576074/get-releases-github-api-v3
            webClient.Headers.Add("User-Agent", "UnityL10nTool");
            try
            {
                switch (onlineUpdate.Selected)
                {
                    case OnlineUpdateCLI.SelectedEnum.Manual:

                        //string tempCurrentVersionPath = "temp\\currentVersion.txt";
                        //if (System.IO.File.Exists(tempCurrentVersionPath))
                        //{
                        //    System.IO.File.Delete(tempCurrentVersionPath);
                        //}
                        //await webClient.DownloadFileTaskAsync(new Uri(onlineUpdate.currentVersionURL), tempCurrentVersionPath);
                        //webClient.DownloadFile(new Uri(onlineUpdate.currentVersionURL), tempCurrentVersionPath);
                        //string currentVersionDownloaded = System.IO.File.ReadAllText(tempCurrentVersionPath);
                        Uri uri = new Uri(onlineUpdate.currentVersionURL);
                        string currentVersionDownloaded = await webClient.DownloadStringTaskAsync(uri);
                        if (onlineUpdate.currentVersion == currentVersionDownloaded)
                        {
                            return false;
                        }
                        else
                        {
                            lastestVersionStr = currentVersionDownloaded;
                            return true;
                        }

                    case OnlineUpdateCLI.SelectedEnum.GitHub:

                        string gitHubApiURL = "https://api.github.com/repos/" + onlineUpdate.gitHubOwner + "/" + onlineUpdate.gitHubRepo + "/releases/latest";
                        string gitHubAPIResult = await webClient.DownloadStringTaskAsync(new Uri(gitHubApiURL));
                        JObject gitHubJson = JObject.Parse(gitHubAPIResult);
                        if (gitHubJson.ContainsKey("tag_name") && gitHubJson.ContainsKey("assets"))
                        {
                            string currentVersionDownload = gitHubJson["tag_name"].ToObject<string>();
                            JToken[] assetsJson = gitHubJson["assets"].ToArray();
                            if (assetsJson.Count() >= 1 && assetsJson[0]["browser_download_url"] != null)
                            {
                                onlineUpdate._gitHubDownloadURL = (string)assetsJson[0]["browser_download_url"];
                                if (currentVersionDownload != onlineUpdate.currentVersion)
                                {
                                    lastestVersionStr = currentVersionDownload;
                                    return true;
                                }
                            }
                        }
                        return false;

                    default:
                        return false;
                }
            }
            catch (ArgumentNullException argumentNullException)
            {
                MessageBox.Show(argumentNullException.Message);
                return false;
            }
            catch (UriFormatException uriFormatException)
            {
                MessageBox.Show(uriFormatException.Message);
                return false;
            }
            catch (WebException webException)
            {
                MessageBox.Show(webException.Message);
                return false;
            }
        }

        private OnlineUpdateCLI ConvertFromJsonToOnlineUpdateCLI(JObject onlineUpdateJson)
        {
            OnlineUpdateCLI onlineUpdateCLI = new OnlineUpdateCLI();
            if (onlineUpdateJson.ContainsKey("currentVersion") && onlineUpdateJson.ContainsKey("Selected"))
            {
                onlineUpdateCLI.currentVersion = onlineUpdateJson["currentVersion"].ToObject<string>();
                string SelectedStr = onlineUpdateJson["Selected"].ToObject<string>();
                switch(SelectedStr)
                {
                    case "Manual":
                        onlineUpdateCLI.Selected = OnlineUpdateCLI.SelectedEnum.Manual;
                        if(onlineUpdateJson.ContainsKey("currentVersionURL") && onlineUpdateJson.ContainsKey("manualZipURL"))
                        {
                            onlineUpdateCLI.currentVersionURL = onlineUpdateJson["currentVersionURL"].ToObject<string>();
                            onlineUpdateCLI.manualZipURL = onlineUpdateJson["manualZipURL"].ToObject<string>();
                        } else
                        {
                            onlineUpdateCLI.Selected = OnlineUpdateCLI.SelectedEnum.None;
                        }
                        break;
                    case "GitHub":
                        onlineUpdateCLI.Selected = OnlineUpdateCLI.SelectedEnum.GitHub;
                        if (onlineUpdateJson.ContainsKey("gitHubOwner") && onlineUpdateJson.ContainsKey("gitHubRepo"))
                        {
                            onlineUpdateCLI.gitHubOwner = onlineUpdateJson["gitHubOwner"].ToObject<string>();
                            onlineUpdateCLI.gitHubRepo = onlineUpdateJson["gitHubRepo"].ToObject<string>();
                        }
                        else
                        {
                            onlineUpdateCLI.Selected = OnlineUpdateCLI.SelectedEnum.None;
                        }
                        break;
                }
            }
            return onlineUpdateCLI;
        }

        // http://ssamlee.tistory.com/38
        static void RenamePrev(string path, string fileExt)
        {
            try
            {
                string[] dirs = Directory.GetDirectories(path);
                string[] files = Directory.GetFiles(path, $"*.{fileExt}");
                foreach (string f in files)
                {
                    // http://infodbbase.tistory.com/118
                    string fileName = System.IO.Path.GetFileNameWithoutExtension(f);
                    string extName = System.IO.Path.GetExtension(f);
                    extName = extName.Substring(1);
                    string folderPath = System.IO.Path.GetDirectoryName(f) + "\\";
                    if (System.IO.File.Exists(folderPath + fileName + "." + fileExt + ".prev"))
                    {
                        System.IO.File.Delete(folderPath + fileName + "." + fileExt + ".prev");
                    }
                    System.IO.File.Move(f, folderPath + fileName + "." + fileExt + ".prev");
                }
                //if (dirs.Length > 0)
                //{
                //    foreach (string dir in dirs)
                //    {
                //        RenamePrev(dir, fileExt);
                //    }
                //}
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        static void DeletePrev(string path, string fileExt)
        {
            try
            {
                string[] dirs = Directory.GetDirectories(path);
                string[] files = Directory.GetFiles(path, $"*.{fileExt}");
                foreach (string f in files)
                {
                    // http://infodbbase.tistory.com/118
                    //string fileName = System.IO.Path.GetFileNameWithoutExtension(f);
                    //string extName = System.IO.Path.GetExtension(f);
                    //extName = extName.Substring(1);
                    //string folderPath = System.IO.Path.GetDirectoryName(f) + "\\";
                    //System.IO.File.Move(f, folderPath + fileName + "." + fileExt + ".prev");
                    System.IO.File.Delete(f);
                }
                //if (dirs.Length > 0)
                //{
                //    foreach (string dir in dirs)
                //    {
                //        RenamePrev(dir, fileExt);
                //    }
                //}
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
            }
        }

        // https://docs.microsoft.com/ko-kr/dotnet/standard/io/how-to-copy-directories
        private static void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs, bool overwrite)
        {
            // Get the subdirectories for the specified directory.
            DirectoryInfo dir = new DirectoryInfo(sourceDirName);

            if (!dir.Exists)
            {
                throw new DirectoryNotFoundException(
                    "Source directory does not exist or could not be found: "
                    + sourceDirName);
            }

            DirectoryInfo[] dirs = dir.GetDirectories();
            // If the destination directory doesn't exist, create it.
            if (!Directory.Exists(destDirName))
            {
                Directory.CreateDirectory(destDirName);
            }

            // Get the files in the directory and copy them to the new location.
            FileInfo[] files = dir.GetFiles();
            foreach (FileInfo file in files)
            {
                string temppath = System.IO.Path.Combine(destDirName, file.Name);
                file.CopyTo(temppath, overwrite);
            }

            // If copying subdirectories, copy them and their contents to new location.
            if (copySubDirs)
            {
                foreach (DirectoryInfo subdir in dirs)
                {
                    string temppath = System.IO.Path.Combine(destDirName, subdir.Name);
                    DirectoryCopy(subdir.FullName, temppath, copySubDirs, overwrite);
                }
            }
        }

        private async Task<bool> PatchLastestVersion(OnlineUpdateCLI onlineUpdateCLI)
        {
            WebClient webClient = new WebClient();
            webClient.DownloadProgressChanged += (s, e) =>
            {
                ProgressBar1.Value = e.ProgressPercentage;
            };
            switch (onlineUpdateCLI.Selected)
            {
                case OnlineUpdateCLI.SelectedEnum.Manual:
                    try
                    {
                        ProgressBar1.IsIndeterminate = false;
                        await webClient.DownloadFileTaskAsync(new Uri(onlineUpdateCLI.manualZipURL), "temp\\lastestVersion.zip");
                    } catch
                    {
                        return false;
                    }
                    break;
                case OnlineUpdateCLI.SelectedEnum.GitHub:
                    try
                    {
                        ProgressBar1.IsIndeterminate = false;
                        await webClient.DownloadFileTaskAsync(new Uri(onlineUpdateCLI._gitHubDownloadURL), "temp\\lastestVersion.zip");
                    }
                    catch
                    {
                        return false;
                    }
                    break;
            }
            if (File.Exists("temp\\lastestVersion.zip"))
            {
                ProgressBar1.IsIndeterminate = true;
                currentStateTextBlock.Text = "Apply Patch";
                ZipFile.ExtractToDirectory("temp\\lastestVersion.zip", "temp\\lastestVersion");
                RenamePrev(AppDomain.CurrentDomain.BaseDirectory, "dll");
                RenamePrev(AppDomain.CurrentDomain.BaseDirectory, "exe");
                RenamePrev(AppDomain.CurrentDomain.BaseDirectory, "pdb");
                DirectoryCopy("temp\\lastestVersion", AppDomain.CurrentDomain.BaseDirectory, true, true);
                return true;
            }
            else
            {
                return false;
            }
        }

        private void Window_Unloaded(object sender, RoutedEventArgs e)
        {
            try
            {
                DeletePrev(AppDomain.CurrentDomain.BaseDirectory, "prev");
            } catch(Exception exception)
            {
                Console.Write(exception);
            }
        }

        private async void Window_ContentRendered(object sender, EventArgs e)
        {
            // https://code.i-harness.com/ko-kr/q/2152283 .Net을 4.5초과로 변경하거나 아래의 작업.
            try {
                System.IO.DirectoryInfo di = new System.IO.DirectoryInfo("temp\\");
                // Delete this dir and all subdirs.
                try
                {
                    di.Delete(true);
                }
                catch (System.IO.IOException) { }
                catch (Exception exception)
                {
                    MessageBox.Show(exception.Message);
                }
                System.IO.Directory.CreateDirectory("temp\\");
                string customSplashFilePath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "customSplash.png");
                if (File.Exists(customSplashFilePath))
                {
                    Uri uri = new Uri(customSplashFilePath, UriKind.RelativeOrAbsolute);
                    SplashImage.Source = BitmapFrame.Create(uri);
                }
                else
                {

                }
                string settingsFilePath = System.IO.Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "setting.json");
                if (File.Exists(settingsFilePath))
                {
                    string jsonStr = System.IO.File.ReadAllText(settingsFilePath);
                    patcherJson = JObject.Parse(jsonStr);
                    if (patcherJson.ContainsKey("OnlineUpdate"))
                    {
                        JObject onlineUpdateJson = (JObject)patcherJson["OnlineUpdate"];
                        OnlineUpdateCLI onlineUpdateCLI = ConvertFromJsonToOnlineUpdateCLI(onlineUpdateJson);
                        bool IsLastestVersionInOnline = await CheckIsLastestVersionInOnline(onlineUpdateCLI);
                        if (IsLastestVersionInOnline)
                        {
                            if (MessageBox.Show(
                                "Lastest patcher is available in online.\nDo you want to download it?\nCurrent version:\t" + onlineUpdateCLI.currentVersion + "\nLastest version:\t" + lastestVersionStr,
                                "Lastest version available",
                                MessageBoxButton.YesNo) == MessageBoxResult.Yes)
                            {
                                currentStateTextBlock.Text = "Download lastest patcher...";
                                string thisExeFilePath = System.Reflection.Assembly.GetEntryAssembly().Location;
                                bool patchedSuccessful = await PatchLastestVersion(onlineUpdateCLI);
                                if (patchedSuccessful)
                                {
                                    // https://stackoverflow.com/questions/8434379/start-new-process-without-being-a-child-of-the-spawning-process
                                    System.Diagnostics.ProcessStartInfo psi = new System.Diagnostics.ProcessStartInfo
                                    {
                                        FileName = thisExeFilePath
                                    };
                                    System.Diagnostics.Process.Start(psi);
                                    Window.GetWindow(this).Close();
                                    return;
                                }
                                else
                                {
                                    currentStateTextBlock.Text = "Patch Fail...";
                                    await Task.Delay(1000);
                                }
                            }
                        }
                        Window mainWindow = new MainWindow();
                        mainWindow.Show();
                        Window.GetWindow(this).Close();
                    }
                    else
                    {
                        await Task.Delay(2000);
                        Window mainWindow = new MainWindow();
                        mainWindow.Show();
                        Window.GetWindow(this).Close();
                    }
                }
                else
                {
                    MessageBox.Show("Cannot Find setting.json.\nPatcher Program will be closed.");
                    Window.GetWindow(this).Close();
                }
            } catch (NotSupportedException notSupportedException)
            {
                MessageBox.Show(notSupportedException.Message);
                await Task.Delay(2000);
                Window mainWindow = new MainWindow();
                mainWindow.Show();
                Window.GetWindow(this).Close();
            }
        }
    }
}
