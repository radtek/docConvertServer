using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;

namespace MyJson
{
    [DataContract]
    public class ResponseData
    {
        [DataMember(Order = 0, IsRequired = true)]
        public int result { get; set; }

        [DataMember(Order = 1, IsRequired = true)]
        public string message { get; set; }

        [DataMember(Order = 2, IsRequired = true)]
        public List<string> data { get; set; }

        [DataMember(Order = 3, IsRequired = false)]
        public int type { get; set; }

    }

    public class ResponseDataPage
    {
        [DataMember(Order = 0, IsRequired = true)]
        public int result { get; set; }

        [DataMember(Order = 1, IsRequired = true)]
        public string message { get; set; }

        [DataMember(Order = 2, IsRequired = true)]
        public int pagecount { get; set; }

        [DataMember(Order = 3, IsRequired = false)]
        public int type { get; set; }

    }

    /// <summary>
    /// 解析JSON，仿Javascript风格
    /// </summary>
    class JSON
    {

        public static T parse<T>(string jsonString)
        {
            using (var ms = new MemoryStream(Encoding.UTF8.GetBytes(jsonString)))
            {
                return (T)new DataContractJsonSerializer(typeof(T)).ReadObject(ms);
            }
        }

        public static string stringify(object jsonObject)
        {
            using (var ms = new MemoryStream())
            {
                new DataContractJsonSerializer(jsonObject.GetType()).WriteObject(ms, jsonObject);
                return Encoding.UTF8.GetString(ms.ToArray());
            }
        }
        
    }
}
